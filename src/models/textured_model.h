#ifndef TEXTURED_MODEL_H // Include guard
#define TEXTURED_MODEL_H

#include "raw_model.h"
#include "../model_texture.h"

class TexturedModel {
private:
    RawModel rawModel;         // The raw model associated with this textured model
    ModelTexture texture;      // The texture associated with this model

public:
    // Constructor to initialize the raw model and texture
    TexturedModel(const RawModel& model, const ModelTexture& texture)
        : rawModel(model), texture(texture) {}

    // Getter for the raw model
    const RawModel& getRawModel() const {
        return rawModel;
    }

    // Getter for the texture
    const ModelTexture& getTexture() const {
        return texture;
    }

    // Overloading the < operator for comparison
    bool operator<(const TexturedModel& other) const {
        // Here you should define a meaningful way to compare two TexturedModel objects
        // For instance, comparing based on a property of the texture or raw model.
        // Assuming ModelTexture has a unique identifier `getID()` method:
        return this->texture.getID() < other.texture.getID(); 
        // Adjust this comparison as necessary based on your actual class structure
    }
};
#endif