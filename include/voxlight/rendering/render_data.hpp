constexpr float cubeVertexData[] = {
    0.0f, 0.0f, 0.0f,  // Vertex 0
    0.0f, 0.0f, 1.0f,  // Vertex 1
    0.0f, 1.0f, 0.0f,  // Vertex 2

    0.0f, 0.0f, 1.0f,  // Vertex 1
    0.0f, 1.0f, 0.0f,  // Vertex 2
    0.0f, 1.0f, 1.0f,  // Vertex 3

    1.0f, 0.0f, 0.0f,  // Vertex 4
    1.0f, 0.0f, 1.0f,  // Vertex 5
    1.0f, 1.0f, 0.0f,  // Vertex 6

    1.0f, 0.0f, 1.0f,  // Vertex 5
    1.0f, 1.0f, 0.0f,  // Vertex 6
    1.0f, 1.0f, 1.0f,  // Vertex 7

    0.0f, 0.0f, 0.0f,  // Vertex 0
    0.0f, 0.0f, 1.0f,  // Vertex 1
    1.0f, 0.0f, 0.0f,  // Vertex 4

    0.0f, 0.0f, 1.0f,  // Vertex 1
    1.0f, 0.0f, 0.0f,  // Vertex 4
    1.0f, 0.0f, 1.0f,  // Vertex 5

    0.0f, 1.0f, 0.0f,  // Vertex 2
    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 1.0f, 0.0f,  // Vertex 6

    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 1.0f, 0.0f,  // Vertex 6
    1.0f, 1.0f, 1.0f,  // Vertex 7

    0.0f, 0.0f, 0.0f,  // Vertex 0
    0.0f, 1.0f, 0.0f,  // Vertex 2
    1.0f, 0.0f, 0.0f,  // Vertex 4

    0.0f, 1.0f, 0.0f,  // Vertex 2
    1.0f, 0.0f, 0.0f,  // Vertex 4
    1.0f, 1.0f, 0.0f,  // Vertex 6

    0.0f, 0.0f, 1.0f,  // Vertex 1
    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 0.0f, 1.0f,  // Vertex 5

    0.0f, 1.0f, 1.0f,  // Vertex 3
    1.0f, 0.0f, 1.0f,  // Vertex 5
    1.0f, 1.0f, 1.0f   // Vertex 7
};

constexpr float quadVertexData[] = {-1.f, -1.f, 0.f, 1.f, 1.f, 0.f, 1.f,  -1.f, 0.f,
                                    -1.f, 1.f,  0.f, 1.f, 1.f, 0.f, -1.f, -1.f, 0.f};
