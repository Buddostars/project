#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->material = material;

    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // Vertex texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::Draw(Shader &shader, unsigned int cubemapTextureID, bool usePBR) 
{
    if (usePBR) {
        // PBR-specific texture binding
        unsigned int albedoNr = 1;
        unsigned int normalNr = 1;
        unsigned int metallicNr = 1;
        unsigned int roughnessNr = 1;
        unsigned int aoNr = 1;

        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding

            std::string number;
            std::string name = textures[i].type;

            if (name == "texture_albedo")
                number = std::to_string(albedoNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_metallic")
                number = std::to_string(metallicNr++);
            else if (name == "texture_roughness")
                number = std::to_string(roughnessNr++);
            else if (name == "texture_ao")
                number = std::to_string(aoNr++);

            shader.setInt(("material." + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
    } else {
        // Non-PBR texture handling
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;

        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding

            std::string number;
            std::string name = textures[i].type;

            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            shader.setInt(("material." + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // Set material properties for non-PBR
        shader.setVec3("material.ambient", material.ambient);
        shader.setVec3("material.diffuse", material.diffuse);
        shader.setVec3("material.specular", material.specular);
        shader.setFloat("material.shininess", material.shininess);
    }

    // Set cubemap texture if it exists
    if (cubemapTextureID != -1) {
        shader.setInt("cubemapTexture", textures.size()); // Bind to the next available texture unit
        shader.setBool("useCubemap", true);
        glActiveTexture(GL_TEXTURE0 + textures.size());
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
    } else {
        shader.setBool("useCubemap", false);
    }

    glActiveTexture(GL_TEXTURE0); // Reset active texture unit

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
