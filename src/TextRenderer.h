#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <map>
#include <string>
#include <glm/glm.hpp>
#include "shader.h"
#include "../dependencies/include/stb_truetype.h"

struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    float Advance;     // Offset to advance to next glyph
};

class TextRenderer {
public:
    // Constructor
    TextRenderer(const char* fontFilePath, Shader& shader);

    // Destructor
    ~TextRenderer();

    // Render text at the given position, scale, and color
    void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color);

    // Calculate the width of the text string
    float CalculateTextWidth(const std::string& text, float scale);

    // Set the projection matrix
    void SetProjection(glm::mat4 projection);

    // Begin using the text shader
    void Use();

private:
    // Shader for rendering text
    Shader& textShader;

    // Holds all state information relevant to a character as loaded using FreeType
    std::map<char, Character> Characters;

    // Render state
    unsigned int VAO, VBO;

    // stb_truetype font info
    stbtt_fontinfo font;
    unsigned char* fontBuffer;

    // Load font from file
    bool LoadFont(const char* fontFilePath);

    // Load character glyphs
    void LoadCharacters();

    // Configure VAO/VBO for texture quads
    void SetupRenderData();
};

#endif // TEXTRENDERER_H