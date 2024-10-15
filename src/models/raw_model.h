#ifndef RAWMODEL_H
#define RAWMODEL_H

class RawModel {
private:
    int vaoID;        // ID of the Vertex Array Object (VAO)
    int vertexCount;  // Number of vertices in the model

public:
    // Constructor to initialize vaoID and vertexCount
    RawModel(int vaoID, int vertexCount)
        : vaoID(vaoID), vertexCount(vertexCount) {}

    // Getter for vaoID
    int getVaoID() const {
        return vaoID;
    }

    // Getter for vertexCount
    int getVertexCount() const {
        return vertexCount;
    }
};
#endif