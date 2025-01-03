#include "TextRenderer.h"
#include <iostream>
#include <fstream>
#include <glad/glad.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "../dependencies/include/stb_truetype.h"

// Constructor with fontSize parameter
TextRenderer::TextRenderer(const char* fontFilePath, Shader& shader, float fontSize) 
    : textShader(shader) {
    if (!LoadFont(fontFilePath)) {
        std::cerr << "Failed to load font: " << fontFilePath << std::endl;
    } else {
        LoadCharacters(fontSize);
        SetupRenderData();
    }
}

// Destructor
TextRenderer::~TextRenderer() {
    // Delete textures
    for (auto& pair : Characters) {
        glDeleteTextures(1, &pair.second.TextureID);
    }
    // Delete VAO and VBO
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Delete font buffer
    delete[] fontBuffer;
}

// Load font from file
bool TextRenderer::LoadFont(const char* fontFilePath) {
    // Open the font file
    std::ifstream fontFile(fontFilePath, std::ios::binary | std::ios::ate);
    if (!fontFile.is_open()) {
        std::cerr << "Could not open font file: " << fontFilePath << std::endl;
        return false;
    }

    // Get file size and read data
    std::streamsize size = fontFile.tellg();
    fontFile.seekg(0, std::ios::beg);

    fontBuffer = new unsigned char[size];
    if (!fontFile.read(reinterpret_cast<char*>(fontBuffer), size)) {
        std::cerr << "Could not read font file: " << fontFilePath << std::endl;
        delete[] fontBuffer;
        return false;
    }
    fontFile.close();

    // Initialise font
    if (!stbtt_InitFont(&font, fontBuffer, stbtt_GetFontOffsetForIndex(fontBuffer, 0))) {
        std::cerr << "Failed to initialise font" << std::endl;
        delete[] fontBuffer;
        return false;
    }

    return true;
}

// Load character glyphs with fontSize
void TextRenderer::LoadCharacters(float fontSize) {
    // Clear previous characters
    Characters.clear();

    // Calculate scale based on font size
    float scale = stbtt_ScaleForPixelHeight(&font, fontSize);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);

    float ascent_scaled = ascent * scale;
    float descent_scaled = descent * scale;

    for (unsigned char c = 32; c < 128; c++) {
        // Load character glyph
        int width, height, xoff, yoff;
        unsigned char* bitmap = stbtt_GetCodepointBitmap(&font, 0, scale, c, &width, &height, &xoff, &yoff);

        if (width == 0 || height == 0) {
            stbtt_FreeBitmap(bitmap, NULL);
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Ensure that pixel rows are tightly packed
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Set texture options
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            width,
            height,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            bitmap
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevent texture wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevent texture wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);    // Linear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    // Linear filtering

        // Get horizontal metrics
        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&font, c, &advanceWidth, &leftSideBearing);

        // Convert metrics to pixels
        float advance = advanceWidth * scale;
        float bearingX = leftSideBearing * scale;

        // Get vertical metrics
        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&font, c, scale, scale, &x0, &y0, &x1, &y1);
        float bearingY = y1 * scale;

        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(width, height),
            glm::vec2(bearingX, bearingY),
            advance
        };
        Characters.insert(std::pair<char, Character>(c, character));

        // Debug output
        // std::cout << "Loaded character '" << c << "': "
        //           << "Width=" << width << ", Height=" << height
        //           << ", BearingX=" << bearingX << ", BearingY=" << bearingY
        //           << ", Advance=" << advance << std::endl;

        // Free glyph bitmap
        stbtt_FreeBitmap(bitmap, NULL);
    }
}

// Configure VAO/VBO for texture quads
void TextRenderer::SetupRenderData() {
    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Dynamic draw because we update VBO memory with each character
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    // Vertex attributes
    glEnableVertexAttribArray(0);
    // (x, y, u, v)
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Render text at the given position, scale, and color
void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
    // Activate corresponding render state
    textShader.use();
    textShader.setVec3("textColor", color);
    textShader.setInt("text", 0); // Ensure the 'text' uniform is set
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Enable blending for transparent text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth testing for text rendering
    glDisable(GL_DEPTH_TEST);

    // Iterate through all characters in the string
    for (unsigned char c : text) {
        // Skip unsupported characters
        if (Characters.find(c) == Characters.end()) {
            // DEBUG
            // std::cout << "Character not found: " << c << std::endl;
            continue;
        }

        Character ch = Characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance cursor to the next glyph
        x += ch.Advance * scale;
    }

    glBindVertexArray(0);

    // Disable blending after rendering
    glDisable(GL_BLEND);

    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
}

// Calculate the width of the text string
float TextRenderer::CalculateTextWidth(const std::string& text, float scale) {
    float width = 0.0f;
    for (unsigned char c : text) {
        if (Characters.find(c) == Characters.end()) continue;
        Character ch = Characters[c];
        width += ch.Advance * scale;
    }
    return width;
}

void TextRenderer::SetProjection(glm::mat4 projection) {
    textShader.use();
    textShader.setMat4("projection", projection);
}

void TextRenderer::Use() {
    textShader.use();
}
