#ifndef TEXT_RENDERER_H_
#define TEXT_RENDERER_H_

#include "window.h"
#include "shaderProgram.h"

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
#include <pango/pangocairo.h>

class TextRenderer {
private:
    using codepoint = msdf_atlas::unicode_t;
    using ustring_base =  std::basic_string<codepoint, std::char_traits<codepoint>, std::allocator<codepoint>>;
    using fcstring_base =  std::basic_string<FcChar8, std::char_traits<FcChar8>, std::allocator<FcChar8>>;

    struct ustring : public ustring_base {
        ustring(const char* chars);
        ustring(const codepoint* chars) : ustring_base(chars) {}
    };

    struct fcstring : public fcstring_base {
        fcstring(const char* chars);
        fcstring(std::string chars) : fcstring(chars.c_str()) {};
        fcstring(const FcChar8* chars) : fcstring_base(chars) {}
    };
public:
    TextRenderer(std::shared_ptr<Window> window);
    void renderText(ustring str, glm::vec2 position, float fontScale, glm::vec4 color);
    void init(std::string font, std::string fontName);
    void setProjection(glm::mat4 projection);
    void setView(glm::mat4 view);

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

    bool                                   _isError;
    DynamicAtlas                           _atlas;
    std::set<codepoint>                    _chars;
    std::vector<msdf_atlas::GlyphGeometry> _glyphs;
    std::map<codepoint, unsigned int>      _glyphIndices;
    std::map<ustring, std::vector<float>>  _glyphCache;

    GLuint                                 _texture;

    PangoAttrList*                         _pangoAttrs;
    PangoContext*                          _pangoContext;
    PangoAttrIterator*                     _pangoAttrIterator;
    PangoFont*                             _pangoFont;

    std::string _getFontPath(std::string fontName);

    void _initMsdf(std::string fontPath);
    void _addGlyphs(ustring codepoints);
    void _renderText(ustring str, glm::vec2 position, float fontScale, glm::vec4 color);
};

#endif