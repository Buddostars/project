#ifndef MODEL_TEXTURE_H // Include guard
#define MODEL_TEXTURE_H
class ModelTexture {
private:
    int textureID;          // ID of the texture
    float shineDamper = 1.0f; // Controls how shiny the object is (default to 1)
    float reflectivity = 0.0f; // Reflectivity of the surface (default to 0)

public:
    // Constructor to initialize the textureID
    ModelTexture(int texture)
        : textureID(texture) {}

    // Getter for the textureID
    int getID() const {
        return textureID;
    }

    // Getter for the shineDamper
    float getShineDamper() const {
        return shineDamper;
    }

    // Setter for the shineDamper
    void setShineDamper(float damper) {
        shineDamper = damper;
    }

    // Getter for the reflectivity
    float getReflectivity() const {
        return reflectivity;
    }

    // Setter for the reflectivity
    void setReflectivity(float reflect) {
        reflectivity = reflect;
    }
};
#endif
