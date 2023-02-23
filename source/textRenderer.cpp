#include "textRenderer.h"
#include "global.h"
#include "util/range.h"
#include <iostream>
#include <set>
#include <thread>

namespace {

const double pixelRange     = 5;
const double glyphScale     = 2.5;
const double miterLimit     = 1.0;
const double maxCornerAngle = 3.0;
const int    threadCount    = 3;

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

void TextRenderer::init(std::string font, std::string fontName) 
{
    auto config = FcConfigCreate();
    fcstring fcstr(Global::GetExeDir() + "/data/fonts");
    FcConfigAppFontAddDir(config, fcstr.c_str());
    FcConfigSetCurrent(config);
    FcInit();

    PangoFontMap* fontMap = pango_cairo_font_map_new_for_font_type(CAIRO_FONT_TYPE_FT);
    _pangoContext = pango_font_map_create_context(fontMap);
    _pangoAttrs = pango_attr_list_new();
    
    auto desc = pango_font_description_from_string(font.c_str());
    _pangoFont = pango_font_map_load_font(fontMap, _pangoContext, desc);

    pango_attr_list_insert(_pangoAttrs, pango_attr_family_new(fontName.c_str()));
    _pangoAttrIterator = pango_attr_list_get_iterator(_pangoAttrs);

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
    
    _initMsdf(_getFontPath(font));
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
}

std::string TextRenderer::_getFontPath(std::string fontName) {
    return Global::GetExeDir() + "/data/fonts/" + fontName;
}

void TextRenderer::_initMsdf(std::string fontPath) 
{
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
        glyph.wrapBox(glyphScale, pixelRange/glyphScale, miterLimit);
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
    _shaderProgram->use();
    _shaderProgram->bindTexture(_texture);
    _shaderProgram->setUniform("fgColor", color);

    std::vector<float> values;

    std::string bytes = "";
    for (auto ch : str) {
        while (ch > 0) {
            const char byte = ch >> 24;
            if (byte != 0) {
                bytes.push_back(byte);
            }
            ch <<= 8;
        }
    }

    const int vertexSize = 4;
    if (_glyphCache.contains(str)) {
        values = _glyphCache[str];
    } else {
        auto scaleAdjustment = 0.98;
        auto items = pango_itemize(_pangoContext, bytes.c_str(), 0, bytes.size(), _pangoAttrs, _pangoAttrIterator);
        do {
            PangoItem* item = (PangoItem*)items->data;
            auto glyphString = pango_glyph_string_new();
            pango_shape_full(&bytes[item->offset], item->num_chars, bytes.c_str(), -1, &item->analysis, glyphString);

            auto glyphs = glyphString->glyphs;
            for (int i = 0; i < glyphString->num_glyphs; i++) {
                auto glyph = glyphs[i];
                auto mGlyph = _glyphs[_glyphIndices[bytes[i]]];


                if (mGlyph.isWhitespace()) {
                    double spaceWidth, tabWidth;
                    msdfgen::getFontWhitespaceWidth(spaceWidth, tabWidth, _font);

                    auto spaceAdjustment = 1.7;

                    if (glyph.glyph == ' ') {
                        position.x += spaceWidth * fontScale * scaleAdjustment / spaceAdjustment;
                    } else {
                        position.x += tabWidth * fontScale * scaleAdjustment / spaceAdjustment;
                    }

                    continue;
                }
                
                Bitmap atlas(_atlas.atlasGenerator().atlasStorage());

                int width, height;
                int atlasWidth = atlas.width();
                int atlasHeight = atlas.height();
                
                double pLeft, pBottom, pRight, pTop;
                double aLeft, aBottom, aRight, aTop;

                mGlyph.getBoxSize(width, height);
                mGlyph.getQuadPlaneBounds(pLeft, pBottom, pRight, pTop);
                mGlyph.getQuadAtlasBounds(aLeft, aBottom, aRight, aTop);

                float rLeft  = position.x + pLeft * fontScale;
                float rTop  = position.y - pTop * fontScale;
                float rRight  = position.x + pRight * fontScale;
                float rBottom  = position.y - pBottom * fontScale;
                                
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
                    rRight, rBottom, right, bottom, // bottom right
                });

                position.x += ((float)glyph.geometry.width / (float)PANGO_SCALE) * fontScale * scaleAdjustment;
            }
            items = items->next;
            pango_glyph_string_free(glyphString);
        } while(items != NULL);

        msdfgen::GlyphIndex lastChar;
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
