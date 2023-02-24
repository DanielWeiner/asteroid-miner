#include "textRenderer.h"
#include "global.h"
#include "util/range.h"
#include <iostream>
#include <set>
#include <thread>
#include <glib-object.h>

namespace {

const int pangoFontSize = 8;
const std::string pangoFontSuffix = " Regular " + std::to_string(pangoFontSize);

const double pixelRange      = 4;
const double glyphScale      = 3;
const double miterLimit      = 1.0;
const double maxCornerAngle  = 3.0;
const int    threadCount     = 3;
const double glyphPadding    = pixelRange/glyphScale;

const char* vertexShaderSource = R"glsl(
/*-------------------VERTEX------------------*/
#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 inTexCoord;

out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * vec4(pos.xy, 0.0, 1.0);
    texCoord = inTexCoord;
}  
/*--------------------------------------------*/
)glsl";

// see: https://discourse.libcinder.org/t/cinder-sdftext-initial-release-wip/171/15
const char* fragmentShaderSource = R"glsl(
/*------------------FRAGMENT------------------*/
#version 330 core

in vec2 texCoord;
out vec4 color;

uniform sampler2D uTex0;

uniform vec4 fgColor;

const float kThickness = 0.125;
const float kNormalization = kThickness * 0.5 * sqrt( 2.0 );

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

vec2 safeNormalize( in vec2 v )
{
   float len = length( v );
   len = ( len > 0.0 ) ? 1.0 / len : 0.0;
   return v * len;
}

void main(void)
{
    // Convert normalized texcoords to absolute texcoords.
    vec2 uv = texCoord * textureSize( uTex0, 0 );
    
    // Calculate derivates
    vec2 Jdx = dFdx( uv );
    vec2 Jdy = dFdy( uv );
    
    // Sample SDF texture (3 channels).
    vec3 sample = texture( uTex0, texCoord ).rgb;

    // calculate signed distance (in texels).
    float sigDist = median( sample.r, sample.g, sample.b ) - 0.5;
    
    // For proper anti-aliasing, we need to calculate signed distance in pixels. We do this using derivatives.
    vec2 gradDist = safeNormalize( vec2( dFdx( sigDist ), dFdy( sigDist ) ) );
    vec2 grad = vec2( gradDist.x * Jdx.x + gradDist.y * Jdy.x, gradDist.x * Jdx.y + gradDist.y * Jdy.y );
    
    // Apply anti-aliasing.
    float afwidth = min( kNormalization * length( grad ), 0.5 );
    float opacity = smoothstep( 0.0 - afwidth, 0.0 + afwidth, sigDist );
    
    // Apply pre-multiplied alpha with gamma correction.
    color.a = pow( fgColor.a * opacity, 1.0 / 2.2 );
    color.rgb = fgColor.rgb * color.a;
}
/*-------------------------------------------*/
)glsl";

}

TextRenderer::TextRenderer(std::shared_ptr<Window> window) : _window(window) {}

void TextRenderer::renderText(ustring str, glm::vec2 position, float fontScale, glm::vec4 color) 
{
    _addGlyphs(str.c_str());
    _renderText(str, position, fontScale, color);
}

void TextRenderer::init(std::string fontName) 
{
    _fontName = fontName;
    _initFontConfig();

    auto pattern = FcPatternBuild (0, FC_FAMILY, FcTypeString, _fontName.c_str(), (char *) 0);
    FcResult result;
    auto foundResult = FcFontMatch(_fcConfig, pattern, &result);
    if (result != FcResult::FcResultMatch) {
        _isError = true;
        return;
    }
    FcValue v;
    FcPatternGet(foundResult, "file", 0, &v);
    std::string fileName((const char*)v.u.s);

    PangoFontMap* fontMap = pango_cairo_font_map_new_for_font_type(CAIRO_FONT_TYPE_FT);    
    _pangoContext = pango_font_map_create_context(fontMap);
    
    auto desc = pango_font_description_new();
    pango_font_description_set_family(desc, fontName.c_str());
    _pangoFont = pango_font_map_load_font(fontMap, _pangoContext, desc);
    pango_font_description_free(desc);

    _atlas.atlasGenerator().setThreadCount(threadCount);
    _shaderProgram = std::make_unique<ShaderProgram>();
    _shaderProgram->init();

    _shaderProgram->addVertexShader(vertexShaderSource);
    _shaderProgram->addFragmentShader(fragmentShaderSource);
    _shaderProgram->linkShaders();
    
    _shaderProgram->initVertexBuffer();
    _shaderProgram->defineAttribute<float>("pos", 2);
    _shaderProgram->defineAttribute<float>("inTexCoord", 2);
    _shaderProgram->bindAttributes();
    
    _initMsdf(fileName);
}

void TextRenderer::setProjection(glm::mat4 projection)
{
    _shaderProgram->use();
    _shaderProgram->setUniform("projection", projection);
}

void TextRenderer::setView(glm::mat4 view) 
{
    _shaderProgram->use();
    _shaderProgram->setUniform("view", view);
}

TextRenderer::~TextRenderer() 
{
    if (_font) {
        msdfgen::destroyFont(_font);
    }
    if (_ft) {
        msdfgen::deinitializeFreetype(_ft);
    }

    if (_pangoContext) {
        g_object_unref(_pangoContext);
    }
    FcConfigDestroy(_fcConfig);
    FcFini();
}

std::string TextRenderer::_getFontPath(std::string fontName) {
    return _getFontDir() + "/" + fontName;
}

std::string TextRenderer::_getFontDir()
{
    return Global::GetExeDir() + "/data/fonts";
}

void TextRenderer::_initFontConfig() 
{
    _fcConfig = FcConfigCreate();
    fcstring fcstr(_getFontDir());
    FcConfigAppFontAddDir(_fcConfig, fcstr.c_str());
    FcConfigSetCurrent(_fcConfig);
    FcInit();
}

void TextRenderer::_initMsdf(std::string fontPath) {
    using namespace msdfgen;
    _ft = initializeFreetype();
    if (!_ft) {
        _isError = true;
        return;
    }
    _font = loadFont(_ft, fontPath.c_str());
    if (!_font) {
        _isError = true;
        return;
    }
}

void TextRenderer::_addGlyphs(ustring codepoints) 
{
    using namespace msdf_atlas;
    using namespace msdfgen;

    int count(codepoints.size());

    std::set<codepoint> newChars;
    if (_chars.empty()) {
        for (auto ch : Charset::ASCII) {
            _chars.insert(ch);
            newChars.insert(ch);
        }
    }

    for (auto i : Util::Range(0, count)) {
        if (!_chars.contains(codepoints[i])) {
            newChars.insert(codepoints[i]);
        }
    }

    if (newChars.empty()) {
        return;
    }
    
    int initialGlyphCount(_glyphs.size());

    for (auto it = newChars.begin(); it != newChars.end(); it++ ) {
        GlyphGeometry glyph;
        glyph.load(_font, 1.0, *it);
        glyph.edgeColoring(&edgeColoringSimple, maxCornerAngle, 0);
        glyph.wrapBox(glyphScale, glyphPadding, miterLimit);
        _glyphIndices[*it] = _glyphs.size();
        _glyphs.push_back(glyph);
    }

    _atlas.add(&_glyphs[initialGlyphCount], _glyphs.size() - initialGlyphCount, false);

    Bitmap storage = (Bitmap)_atlas.atlasGenerator().atlasStorage();

    _shaderProgram->use();
    _texture = _shaderProgram->loadTexture(
        storage(0,0), 
        storage.width(),
        storage.height(),
        3,
        true
    );

    _shaderProgram->unbindVao();
}

void TextRenderer::_renderText(ustring str, glm::vec2 position, float fontScale, glm::vec4 color) 
{
    const double pangoScale = PANGO_SCALE / fontScale;
    const int vertexSize = 4;

    _shaderProgram->use();
    _shaderProgram->bindTexture(_texture);
    _shaderProgram->setUniform("fgColor", color);

    std::vector<float> values;
    std::string bytes(str.begin(), str.end());
    
    if (_glyphCache.contains(str)) {
        values = _glyphCache[str];
    } else {
        auto layout = pango_layout_new(_pangoContext);

        auto desc = pango_font_description_new();
        pango_font_description_set_family(desc, _fontName.c_str());
        pango_layout_set_font_description(layout, desc);
        pango_font_description_free(desc);

        pango_layout_set_text(layout, bytes.c_str(), bytes.size());
        
        auto iter = pango_layout_get_iter(layout);
        do {
            auto charIndex = pango_layout_iter_get_index(iter);
            auto byte = bytes[charIndex];
            if (byte == ' ' || byte == '\t') {
                continue;
            }

            auto mGlyph = _glyphs[_glyphIndices[byte]];

            PangoRectangle logicalRect;
            pango_layout_iter_get_cluster_extents(iter, NULL, &logicalRect);
            
            Bitmap atlas(_atlas.atlasGenerator().atlasStorage());

            int atlasWidth = atlas.width();
            int atlasHeight = atlas.height();
            
            double aLeft, aBottom, aRight, aTop,
                   pLeft, pBottom, pRight, pTop;

            mGlyph.getQuadAtlasBounds(aLeft, aBottom, aRight, aTop);
            mGlyph.getQuadPlaneBounds(pLeft, pBottom, pRight, pTop);
            
            double baseline = pango_layout_iter_get_baseline(iter) / pangoScale;

            float lLeft  = logicalRect.x / pangoScale + position.x;
            float lTop  = logicalRect.y / pangoScale + position.y;

            float rLeft  = lLeft + pLeft * fontScale;
            float rTop  = lTop - pTop * fontScale + baseline;
            float rRight  = lLeft + pRight * fontScale;
            float rBottom  = lTop - pBottom * fontScale + baseline;
            
            float left = aLeft / atlasWidth;
            float top = aTop / atlasHeight;
            float right = aRight / atlasWidth;
            float bottom = aBottom / atlasHeight;

            values.insert(values.end(), { 
                rLeft,  rTop,    left, top,    // top left
                rRight, rTop,    right, top,   // top right
                rLeft,  rBottom, left, bottom, // bottom left
                
                rRight, rTop,    right, top,    // top right
                rLeft,  rBottom, left, bottom,  // bottom left
                rRight, rBottom, right, bottom  // bottom right
            });
        } while (pango_layout_iter_next_cluster(iter));

        pango_layout_iter_free(iter);
        g_object_unref(layout);

        _glyphCache[str] = values;
    }
    
    _shaderProgram->loadData(values.size() * sizeof(float), values.size() / vertexSize, &values[0]);
    _shaderProgram->drawArrays();
    _shaderProgram->unbindTextures();
    _shaderProgram->unbindVao();
}

TextRenderer::ustring::ustring(const char* chars)
{
    std::string str(chars);
    for (auto ch : str) {
        (*this).push_back((codepoint)ch);
    }
}

TextRenderer::fcstring::fcstring(const char *chars) 
{
    std::string str(chars);
    for (auto ch : str) {
        (*this).push_back((FcChar8)ch);
    }
}
