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

class TextRenderer {
private:
    using codepoint = msdf_atlas::unicode_t;
    using ustring_base =  std::basic_string<codepoint, char_traits<codepoint>, allocator<codepoint>>;

    struct ustring : public ustring_base {
        ustring(const char* chars);
        ustring(const codepoint* chars) : ustring_base(chars) {}
    };
public:
    TextRenderer(std::shared_ptr<Window> window);
    void renderText(ustring str, glm::vec2 position, float fontScale, glm::vec4 color);
    void init(std::string font);
    void setProjection(glm::mat4 projection);
    void setView(glm::mat4 view);

    static void msdfGenerator(const msdfgen::BitmapRef<float, 4> &output, const msdf_atlas::GlyphGeometry &glyph, const msdf_atlas::GeneratorAttributes &attribs);

    ~TextRenderer();
private:

    using Bitmap = msdfgen::Bitmap<msdf_atlas::byte, 4>;
    using AtlasStorage = msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 4>;
    using AtlasGenerator = msdf_atlas::ImmediateAtlasGenerator<float, 4, &TextRenderer::msdfGenerator, AtlasStorage>;
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

    std::string _getFontPath(std::string fontName);

    void _initMsdf(std::string fontPath);
    void _addGlyphs(ustring codepoints);
    void _renderText(ustring str, glm::vec2 position, float fontScale, glm::vec4 color);
};

#endif