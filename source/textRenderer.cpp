#include "textRenderer.h"
#include "global.h"
#include "util/range.h"
#include "constants.h"

#include <glib-object.h>
#include <glm/ext.hpp>
#include <pango/pangocairo.h>
#include <algorithm>

namespace {
// scaling factor to help pango to properly layout text at subpixel ranges
const double internalPangoScale = 10;

const double pixelRange      = 8;
const double glyphScale      = 3;
const double miterLimit      = 5.0;
const double maxCornerAngle  = 5.0;
const int    threadCount     = 3;
const double glyphPadding    = pixelRange / glyphScale;

const char* vertexShaderSource = R"glsl(
/*-------------------VERTEX------------------*/
#version 430 core

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
#version 430 core

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
    vec3 msdf = texture( uTex0, texCoord ).rgb;

    // calculate signed distance (in texels).
    float sigDist = median( msdf.r, msdf.g, msdf.b ) - 0.5;
    
    // For proper anti-aliasing, we need to calculate signed distance in pixels. We do this using derivatives.
    vec2 gradDist = safeNormalize( vec2( dFdx( sigDist ), dFdy( sigDist ) ) );
    vec2 grad = vec2( gradDist.x * Jdx.x + gradDist.y * Jdy.x, gradDist.x * Jdx.y + gradDist.y * Jdy.y );
    
    // Apply anti-aliasing.
    float afwidth = min( kNormalization * length( grad ), 0.5 );
    float opacity = smoothstep( 0.0 - afwidth, 0.0 + afwidth, sigDist );
    
    // Apply pre-multiplied alpha with gamma correction.
    color.a = pow( fgColor.a * opacity, 1.0 / 1.2 );
    color.rgb = fgColor.rgb * color.a;
}
/*-------------------------------------------*/
)glsl";

}

TextRenderer::TextRenderer(std::shared_ptr<Window> window) : _window(window) {}

void TextRenderer::createLayout(std::string str, double width, TextRenderer::Layout& layout, Layout::Alignment alignment, bool justify)
{
    _addGlyphs(str.c_str());
    std::string bytes(str.begin(), str.end());

    auto pangoLayout = pango_layout_new(_pangoContext);
    pango_layout_set_width(pangoLayout, width * internalPangoScale * PANGO_SCALE);
    pango_layout_set_alignment(pangoLayout, alignment);
    pango_layout_set_justify(pangoLayout, justify);

    auto desc = pango_font_description_new();
    pango_font_description_set_family(desc, _fontName.c_str());
    pango_layout_set_font_description(pangoLayout, desc);
    pango_font_description_free(desc);

    auto attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attr_scale_new(internalPangoScale));
    pango_layout_set_attributes(pangoLayout, attrs);
    pango_attr_list_unref(attrs);

    pango_layout_set_text(pangoLayout, bytes.c_str(), bytes.size());
    Bitmap atlas(_atlas.atlasGenerator().atlasStorage());

    int atlasWidth = atlas.width();
    int atlasHeight = atlas.height();
    
    auto iter = pango_layout_get_iter(pangoLayout);

    PangoRectangle firstLineRect;
    pango_layout_iter_get_line_extents(iter, &firstLineRect, NULL);
    double initialHeight = PANGO_PIXELS(firstLineRect.y + firstLineRect.height / 2) / internalPangoScale;
    
    PangoGlyphItemIter clusterIter;
    float textLeft = MAX_FLOAT;
    float textRight = MIN_FLOAT;
    float textTop = MAX_FLOAT;
    float textBottom = MIN_FLOAT;

    layout._dirty = true;

    do {
        auto run = pango_layout_iter_get_run_readonly(iter);
        if (run == NULL) {
            continue;
        }

        PangoItem pangoItem = *run->item;
        std::unique_ptr<int[]> widths(new int[pangoItem.length + 1]);
        pango_glyph_item_get_logical_widths(run, &bytes[pangoItem.offset], widths.get());
        
        pango_glyph_item_iter_init_start(&clusterIter, run, bytes.c_str());

        PangoRectangle lineRect;
        pango_layout_iter_get_line_extents(iter, NULL, &lineRect);
        pango_extents_to_pixels(&lineRect, NULL);
        int y = lineRect.y;
        int lineX = lineRect.x;
        do {
            PangoGlyph pangoGlyph = run->glyphs->glyphs[clusterIter.start_glyph].glyph;
            if (!_glyphIndices.contains(pangoGlyph)) {
                continue;
            }

            int x;
            pango_glyph_string_index_to_x(run->glyphs, &bytes[pangoItem.offset], pangoItem.length, &pangoItem.analysis, clusterIter.start_char, false, &x);
            x = PANGO_PIXELS(x) + lineX;

            auto mGlyph = _glyphGeometries[_glyphIndices[pangoGlyph]];

            PangoRectangle logicalRect;
        
            pango_font_get_glyph_extents(_pangoFont, clusterIter.start_glyph, NULL, &logicalRect);
            pango_extents_to_pixels(NULL, &logicalRect);
            
            double atlasLeft, atlasBottom, atlasRight, atlasTop,
                   planeLeft, planeBottom, planeRight, planeTop;

            mGlyph.getQuadAtlasBounds(atlasLeft, atlasBottom, atlasRight, atlasTop);
            mGlyph.getQuadPlaneBounds(planeLeft, planeBottom, planeRight, planeTop);
            
            float logicalLeft  = (logicalRect.x + x) / internalPangoScale;
            float logicalTop  = (logicalRect.y + y) / internalPangoScale + initialHeight;

            float vertexLeft = logicalLeft + planeLeft;
            float vertexTop = logicalTop - planeTop;
            float vertexRight = logicalLeft + planeRight;
            float vertexBottom = logicalTop - planeBottom;
            
            float scaledAtlasLeft = atlasLeft / atlasWidth;
            float scaledAtlasTop = atlasTop / atlasHeight;
            float scaledAtlasRight = atlasRight / atlasWidth;
            float scaledAtlasBottom = atlasBottom / atlasHeight;

            layout._count += 6;
            layout._vertices.insert(layout._vertices.end(), { 
                vertexLeft,  vertexTop,    scaledAtlasLeft,  scaledAtlasTop,    // top left
                vertexRight, vertexTop,    scaledAtlasRight, scaledAtlasTop,    // top right
                vertexLeft,  vertexBottom, scaledAtlasLeft,  scaledAtlasBottom, // bottom left
                
                vertexRight, vertexTop,    scaledAtlasRight, scaledAtlasTop,    // top right
                vertexLeft,  vertexBottom, scaledAtlasLeft,  scaledAtlasBottom, // bottom left
                vertexRight, vertexBottom, scaledAtlasRight, scaledAtlasBottom  // bottom right
            });

            textRight = std::max(textRight, vertexRight);
            textLeft = std::min(textLeft, vertexLeft);
            textBottom = std::max(textBottom, vertexBottom);
            textTop = std::min(textTop, vertexTop);
        } while(pango_glyph_item_iter_next_cluster(&clusterIter));
    } while (pango_layout_iter_next_run(iter));

    pango_glyph_item_iter_free(&clusterIter);
    pango_layout_iter_free(iter);
    g_object_unref(pangoLayout);

    layout._size = glm::vec2(textRight - textLeft, (textBottom - textTop));
}

void TextRenderer::renderLayout(TextRenderer::Layout& layout, glm::vec2 position, float fontScale, glm::vec4 color) 
{
    if (layout._vertices.size() == 0) {
        return;
    }
    
    glm::mat4 view(1.0);
    view = glm::translate(view, glm::vec3(position, 0.f));
    view = glm::scale(view, glm::vec3(glm::vec2(fontScale), 1.f));

    _shaderProgram->use();
    _shaderProgram->bindTexture(_texture);
    _shaderProgram->setUniform("fgColor", color);
    _shaderProgram->setUniform("view", view);
    if (layout._dirty) {
        layout._dirty = false;
        _shaderProgram->loadData(layout._vertices.size() * sizeof(float), layout._count, layout._vertices.data());
    }
    _shaderProgram->drawArrays();
    _shaderProgram->unbindTextures();
    _shaderProgram->unbindVao();
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
    
    _fontPath = fileName;
    _initMsdf();
}

void TextRenderer::setProjection(glm::mat4 projection)
{
    _shaderProgram->use();
    _shaderProgram->setUniform("projection", projection);
}

glm::vec2 TextRenderer::getLayoutSize(Layout& layout)
{
    if (layout._vertices.size()) {
        return layout._size;
    }
    return glm::vec2(0);
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
    fcstring fontDir(_getFontDir());
    FcConfigAppFontAddDir(_fcConfig, fontDir.c_str());
    FcConfigSetCurrent(_fcConfig);
    FcInit();
}

void TextRenderer::_initMsdf() {
    using namespace msdfgen;
    _ft = initializeFreetype();
    if (!_ft) {
        _isError = true;
        return;
    }
    _font = loadFont(_ft, _fontPath.c_str());
    if (!_font) {
        _isError = true;
        return;
    }
}

void TextRenderer::_addGlyphs(std::string bytes) 
{
    using namespace msdf_atlas;
    using namespace msdfgen;

    std::set<std::string> newChars;

    if (_glyphs.empty()) {
        std::string ascii = "";
        for (const char ch : Charset::ASCII) {
            const char cStr[]{ ch, '\0' };
            std::string str(cStr);
            ascii += cStr;
        }
        bytes = ascii + bytes;
    }

    auto layout = pango_layout_new(_pangoContext);
    auto desc = pango_font_description_new();
    pango_font_description_set_family(desc, _fontName.c_str());
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    pango_layout_set_text(layout, bytes.c_str(), bytes.size());
    
    auto layoutIter = pango_layout_get_iter(layout);
    PangoGlyphItemIter clusterIter;

    int initialGlyphCount(_glyphGeometries.size());
    do {
        auto run = pango_layout_iter_get_run_readonly(layoutIter);
        if (run == NULL) {
            continue;
        }

        pango_glyph_item_iter_init_start(&clusterIter, run, bytes.c_str());

        do {
            std::string glyphStr(bytes.begin() + clusterIter.start_char, bytes.begin() + clusterIter.end_char);
            if (glyphStr == " " || glyphStr == "\t" || glyphStr == "\r" || glyphStr == "\n") {
                continue;
            }
            PangoGlyph pangoGlyph = run->glyphs->glyphs[clusterIter.start_glyph].glyph;
            if (_glyphIndices.contains(pangoGlyph)) {
                continue;
            }

            GlyphGeometry glyph;

            glyph.load(_font, 1.0, GlyphIndex(pangoGlyph)); 
            glyph.edgeColoring(&edgeColoringByDistance, maxCornerAngle, 0);
            glyph.wrapBox(glyphScale, glyphPadding, miterLimit);
            _glyphIndices[pangoGlyph] = _glyphGeometries.size();
            _glyphGeometries.push_back(glyph);
        } while(pango_glyph_item_iter_next_cluster(&clusterIter));
    } while (pango_layout_iter_next_run(layoutIter));

    pango_glyph_item_iter_free(&clusterIter);
    pango_layout_iter_free(layoutIter);
    g_object_unref(layout);

    if (_glyphGeometries.size() == initialGlyphCount) {
        return;
    }

    _atlas.add(&_glyphGeometries[initialGlyphCount], _glyphGeometries.size() - initialGlyphCount, false);

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