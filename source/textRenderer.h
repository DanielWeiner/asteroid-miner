#ifndef TEXT_RENDERER_H_
#define TEXT_RENDERER_H_

#include "window.h"
#include "shaderProgram.h"
#include "util/string.h"

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <vector>
#include <set>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdf-atlas-gen/types.h>
#include <fontconfig/fontconfig.h>
#include <pango/pango.h>

class TextRenderer {
private:
    using codepoint = msdf_atlas::unicode_t;

public:
    struct Layout {
    friend TextRenderer;
    private:
        std::vector<float> vertices;
    };
    using fcstring = Util::String::String<FcChar8>;

    TextRenderer(std::shared_ptr<Window> window);
    void createLayout(std::string str, int width,Layout& layout);
    void renderLayout(Layout& layout, glm::vec2 position, float fontScale, glm::vec4 color);
    void init(std::string fontName);
    void setProjection(glm::mat4 projection);

    ~TextRenderer();
private:

    using Bitmap = msdfgen::Bitmap<msdf_atlas::byte, 3>;
    using AtlasStorage = msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>;
    using AtlasGenerator = msdf_atlas::ImmediateAtlasGenerator<float, 3, &msdf_atlas::msdfGenerator, AtlasStorage>;
    using DynamicAtlas = msdf_atlas::DynamicAtlas<AtlasGenerator>;

    std::shared_ptr<Window>                _window;
    std::unique_ptr<ShaderProgram>         _shaderProgram;

    msdfgen::FreetypeHandle*               _ft;
    msdfgen::FontHandle*                   _font;

    std::string                            _fontName;

    bool                                   _isError;
    DynamicAtlas                           _atlas;
    std::set<PangoGlyph>                   _glyphs;
    std::vector<msdf_atlas::GlyphGeometry> _glyphGeometries;
    std::map<PangoGlyph, unsigned int>     _glyphIndices;

    GLuint                                 _texture;

    PangoContext*                          _pangoContext;
    PangoFont*                             _pangoFont;

    FcConfig*                              _fcConfig;
    std::string                            _fontPath;

    std::string _getFontPath(std::string fontName);
    std::string _getFontDir();

    void _initFontConfig();
    void _initMsdf();
    void _addGlyphs(std::string codepoints);
};

#endif