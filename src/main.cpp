#include <SFML/Graphics.hpp>


void handleInput();

class Triangle {

public:
    Triangle(sf::Vertex a, sf::Vertex b, sf::Vertex c) {

        a.color = sf::Color::Red;
        b.color = sf::Color::Red;
        c.color = sf::Color::Red;

        triangle[0] = a;
        triangle[1] = b;
        triangle[2] = c;
        triangle[3] = a;
    }

    sf::Vertex triangle[4];
};

std::vector<sf::Vertex> vertices;
std::vector<Triangle> triangles;
sf::RenderWindow window(sf::VideoMode(800, 600), "Triangulation");

bool flag = true;
bool mouseFlag = false;
bool showTriangles = false;

int main() {

    //vertices.emplace_back(sf::Vertex(sf::Vector2f(50, 50), sf::Color::Red));
    //vertices.emplace_back(sf::Vertex(sf::Vector2f(50, 100), sf::Color::Green));
    //vertices.emplace_back(sf::Vertex(sf::Vector2f(100, 50), sf::Color::Blue));
    //vertices.emplace_back(sf::Vertex(sf::Vector2f(50, 50), sf::Color::Red));

    while(window.isOpen()) {

        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
            if(event.type == sf::Event::MouseButtonReleased && mouseFlag) {
                sf::Vector2f pos = sf::Vector2f(sf::Mouse::getPosition(window));

                if(!flag) {
                    vertices.insert(vertices.end() - 1, sf::Vertex(pos));
                }
                else {
                    vertices.emplace_back(sf::Vertex(pos));
                    vertices.emplace_back(sf::Vertex(pos));
                    flag = false;
                }
                mouseFlag = false;
            }
        }

        handleInput();

        window.clear(sf::Color::Black);

        window.draw(vertices.data(), vertices.size(), sf::LinesStrip);

        if(showTriangles) {
            window.setFramerateLimit(1);
            for(Triangle t : triangles) {
                window.draw(t.triangle, 4, sf::LinesStrip);
                window.display();
            }

        }

        window.display();

    }

    return 0;
}

//Gets a valid list index even if the actual index is out of bounds
int getItem(int ai, int amax) {
    int result = ai % amax;

    if(result < 0) {
        result = amax + result;
    }

    return result;
}

//checks if a point is in a triangle
bool isPointInTriangle(sf::Vertex ap1, sf::Vertex tp1, sf::Vertex tp2, sf::Vertex tp3) {
    double b0, b1, b2, b3;

    b0 = ((tp2.position.x - tp1.position.x) * (tp3.position.y - tp1.position.y) - (tp3.position.x - tp1.position.x) * (tp2.position.y - tp1.position.y));

    if(b0 != 0) {
        b1 = (((tp2.position.x - ap1.position.x) * (tp3.position.y - ap1.position.y) - (tp3.position.x - ap1.position.x) * (tp2.position.y - ap1.position.y)) / b0);
        b2 = (((tp3.position.x - ap1.position.x) * (tp1.position.y - ap1.position.y) - (tp1.position.x - ap1.position.x) * (tp3.position.y - ap1.position.y)) / b0);
        b3 = 1 - b1 - b2;
        return (b1 > 0) && (b2 > 0) && (b3 > 0);
    }
    return false;
}

bool triangulate() {

    std::vector<sf::Vertex> lst;

    for(int i = 0; i < vertices.size() - 1; i++) {
        lst.emplace_back(vertices[i]);
    }

    int i = 0;
    int lastEar = -1;

    do {

        lastEar++;

        sf::Vertex p1 = lst[getItem(i - 1, lst.size())];
        sf::Vertex p = lst[getItem(i, lst.size())];
        sf::Vertex p2 = lst[getItem(i + 1, lst.size())];

        float l = ((p1.position.x - p.position.x) * (p2.position.y - p.position.y) - (p1.position.y - p.position.y) * (p2.position.x - p.position.x));

        if(l < 0) {
            bool inTriangle = false;

            for(int j = 2; j <= lst.size() - 2; j++) {
                sf::Vertex pt = lst[getItem(i + j, lst.size())];
                if(isPointInTriangle(pt, p1, p, p2)) {
                    inTriangle = true;
                    break;
                }
            }

            if(!inTriangle) {
                triangles.emplace_back(Triangle(p1, p, p2));

                lst.erase(lst.begin() + getItem(i, lst.size()));

                lastEar = 0;

                i--;
            }

        }

        i++;

        if(i > lst.size() - 1) i = 0;

    } while((lastEar < lst.size() * 2) && (lst.size() != 3));

    if(lst.size() == 3) {
        sf::Vertex p1 = lst[getItem(0, lst.size())];
        sf::Vertex p = lst[getItem(1, lst.size())];
        sf::Vertex p2 = lst[getItem(2, lst.size())];

        triangles.emplace_back(Triangle(p1, p, p2));
    }

    return lst.size() == 3;

}

void handleInput() {

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        mouseFlag = true;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
        vertices.clear();
        flag = true;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
        showTriangles = triangulate();
    }

}
