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
#include <map>

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
        using Alignment = PangoAlignment;
        struct ALIGN {
            static constexpr Alignment LEFT = PANGO_ALIGN_LEFT;
            static constexpr Alignment RIGHT = PANGO_ALIGN_RIGHT;
            static constexpr Alignment CENTER = PANGO_ALIGN_CENTER;
        };
    friend TextRenderer;
    private:
        bool               _dirty{false};
        glm::vec2          _size;
        std::vector<float> _vertices;
        unsigned int       _count;
    };
    using fcstring = Util::String::String<FcChar8>;

    TextRenderer(Window& window);
    void createLayout(std::string str, double width, Layout& layout, Layout::Alignment alignment, bool justify);
    void renderLayout(Layout& layout, glm::vec2 position, float fontScale, glm::vec4 color);
    void init(std::string fontName);
    void setProjection(glm::mat4 projection);
    glm::vec2 getLayoutSize(Layout& layout);

    ~TextRenderer();
private:

    using Bitmap = msdfgen::Bitmap<msdf_atlas::byte, 3>;
    using AtlasStorage = msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>;
    using AtlasGenerator = msdf_atlas::ImmediateAtlasGenerator<float, 3, &msdf_atlas::msdfGenerator, AtlasStorage>;
    using DynamicAtlas = msdf_atlas::DynamicAtlas<AtlasGenerator>;

    Window                                 _window;
    ShaderProgram                          _shaderProgram;

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