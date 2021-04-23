#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <string>

using std::vector;
using std::cout;
using std::endl;


//Global variables
unsigned int height = sf::VideoMode::getDesktopMode().height;
unsigned int width = sf::VideoMode::getDesktopMode().width;
float base_size = (float)width / 32;
int energy = 100;
vector<int> price_of_animal = {0, 5, 10, 30};
int selected_type = 0; //1 - simple, 2 - shouter
sf::Font font;
sf::Color green = sf::Color::Green;
sf::Color red = sf::Color::Red;
sf::Color white = sf::Color::White;


class Base;

//Point class
class Point{
public:
    Point() = default;
    Point(double x_, double y_){
        x = x_;
        y = y_;
    }
    [[nodiscard]] double get_x() const {return x;}
    [[nodiscard]] double get_y() const {return y;}
    void set_x(double a) {x = a;}
    void set_y(double a) {y = a;}
    [[nodiscard]] double delta_x(Point a) const;
    [[nodiscard]] double delta_y(Point a) const;
    [[nodiscard]] double distance(Point a) const;
    Point& operator= (Point a){
        this->set_x(a.get_x());
        this->set_y(a.get_y());
        return *this;
    }
private:
    double x = 0;
    double y = 0;
};

double Point::delta_x(Point a) const{
    return -(get_x() - a.get_x());
}

double Point::delta_y(Point a) const{
    return -(get_y() - a.get_y());
}

double Point::distance(Point a) const{
    return sqrt(delta_x(a) * delta_x(a) + delta_y(a) * delta_y(a));
}



//Animal class----------------------------------------------------------------------------------------------------------
class Animal{
public:
    Animal(int energy_, int strength_, int speed_, Point aim_, Point pos_){
        energy = 0;
        strength = strength_;
        speed = speed_;
        aim = aim_;
        pos = pos_;
        color = green;
        selected = false;
        price = 0;
        size = width / 120;
        type = 0;
    }
    virtual ~Animal() = default;
    void move() {
        if(pos.distance(aim) < this->size*2) stable = true;
        pos.set_x(floor(pos.get_x() + speed * pos.delta_x(aim) / pos.distance(aim)));
        pos.set_y(floor(pos.get_y() + speed * pos.delta_y(aim) / pos.distance(aim)));
    }
    virtual void draw() = 0;
    virtual void attack(Animal* opponent) = 0;
    virtual void capture(Base* base) = 0;
    int get_energy() const{return energy;}
    int get_strength() const{return strength;}
    int get_price() const{return price;}
    int get_speed() const{return speed;}
    int get_type() const{return type;}
    bool is_selected() const{return selected;}
    void set_energy(int energy_) {energy = energy_;}
    void set_strength(int strength_) {strength = strength_;}
    void set_price(int price_) {price = price_;}
    void set_speed(int speed_) {speed = speed_;}
    void select(bool a) {selected = a;}
    Point get_pos() const {return pos;}
    void set_pos(Point a) {pos = a;}
    Point get_aim() const {return aim;}
    void set_aim(Point a) {aim = a;}
    sf::CircleShape picture;
    bool stable = true;
    sf::Color color;
    Point pos, aim = pos;
    int size;
protected:
    int energy, strength, price, speed, type;
    bool selected;

};

class Bullet{
protected:
    int speed;
    int damage;
    int lifetime = 255;
    sf::Color color = sf::Color(0, 0, 0, lifetime);
    double cosinus;
    double sinus;
public:
    Bullet(int speed_, int damage_, int lifetime_, Point pos_, Point aim_){
        speed = speed_;
        damage = damage_;
        lifetime = lifetime_;
        pos = pos_;
        aim = aim_;
        cosinus = pos.delta_x(aim) / pos.distance(aim);
        sinus = pos.delta_y(aim) / pos.distance(aim);
        this->picture.setRadius(5);
        this->picture.setPosition(this->pos.get_x(), this->pos.get_y());
        this->picture.setOrigin(5, 5);
        this->picture.setFillColor(this->color);
        this->picture.setOutlineThickness(0);
        this->picture.setOutlineColor(white);
    }
    void move(){
        pos.set_x(pos.get_x() + speed * cosinus);
        pos.set_y(pos.get_y() + speed * sinus);
        lifetime -= 1;
    }
    ~Bullet() = default;
    void hit(Animal* animal){
        animal->set_energy(animal->get_energy() - damage);
        delete this;
    }
    Point pos;
    Point aim;
    sf::CircleShape picture;
};

//Animal types:

//Simple_Animal

class Simple_Animal: public Animal
{
public:
    Simple_Animal(int energy_, int strength_, int speed_, Point aim_, Point pos_):
    Animal(energy_, strength_, speed_, aim_, pos_){
        price = 15;
        strength = 10;
        speed = speed_;
        size = width / 120;
        type = 1;
    }
    void attack(Animal* opponent) final;
    void capture(Base* base) final;
    void draw() final;
};

vector<Animal*> simple_animals = {};
vector<Animal*> enemy_animals = {};
vector<Bullet*> bullets = {};

void Simple_Animal::draw() {
    this->picture.setRadius(this->size);
    this->picture.setPosition(this->get_pos().get_x(), this->get_pos().get_y());
    this->picture.setOrigin(this->size, this->size);
    this->picture.setFillColor(this->color);
    this->picture.setOutlineThickness(0);
    this->picture.setOutlineColor(white);
}

void Simple_Animal::attack(Animal* opponent){
    opponent->set_energy(opponent->get_energy() - strength);
    energy -= opponent->get_strength();
    if (energy < 0){
        delete this;
    }
}

void Simple_Animal::capture(Base* base){
}


//Shouter_Animal
class Shouter_Animal: public Animal
{
public:
    Shouter_Animal(int energy_, int strength_, int speed_, Point aim_, Point pos_):
            Animal(energy_, strength_, speed_, aim_, pos_){
        price = 25;
        strength = 5;
        speed = speed_;
        size = width / 100;
        type = 2;
    }
    void attack(Animal* opponent) final;
    void capture(Base* base) final;
    void draw() final;
};

void Shouter_Animal::draw() {
    this->picture.setRadius(this->size);
    this->picture.setPosition(this->get_pos().get_x(), this->get_pos().get_y());
    this->picture.setOrigin(this->size, this->size);
    this->picture.setFillColor(this->color);
    this->picture.setOutlineThickness(0);
    this->picture.setOutlineColor(white);
}

void Shouter_Animal::attack(Animal *opponent) {
    auto bullet = new Bullet(20, 50, 255, pos, aim);
    bullets.push_back(bullet);
}

void Shouter_Animal::capture(Base* base) {

}


//BaseMenu class--------------------------------------------------------
class BaseMenu{
public:
    BaseMenu() = default;
    sf::RectangleShape picture;
    sf::Color color;
    Point pos;
};

//Base class--------------------------------------------------------
class Base{
public:
    sf::CircleShape picture;
    sf::Color color;
    Point pos;
    BaseMenu menu;
    int num_of_animals;
    bool is_selected = false;
    int radius;

    explicit Base(Point pos_){
        pos = pos_;
        color = white;
        radius = (int)base_size;
        num_of_animals = 0;
    }
    bool is_in_base(Point a) const;
};

bool::Base::is_in_base(Point a) const{
    return (a.distance(this->pos) * a.distance(this->pos) < radius * radius);
}

Point p1(width / 16, height / 9), p2(width / 4, height * 4 / 9), p3(width / 8, height * 7 / 9),
        p4(width / 2, height / 3), p5(width * 3 / 4, height * 4 / 9),
        p6(width * 7 / 8, height * 7 / 9), p7(width * 15 / 16, height / 9);
Base b1(p1), b2(p2), b3(p3), b4(p4), b5(p5), b6(p6), b7(p7);
vector<Base> bases = {b1, b2, b3, b4, b5, b6, b7};


struct button{
    sf::RectangleShape picture;
    sf:: Text caption;
    bool pushed = false;
};
//Board structure-----------------------------------------------------
struct Board{
    sf::RectangleShape energy_lvl;
    sf::RectangleShape energy_lvl_back;
    sf::RectangleShape board;
    button spawn_base;
    sf::Text energy_lvl_caption;
};

//Game class--------------------------------------------------------
class Game
{
private:
    //Private variables
    sf::RenderWindow* window{};
    sf::VideoMode videoMode;
    sf::Event ev{};
    sf::RectangleShape cursor;
    Board board;
    sf::RectangleShape area;
    Point mouse;
    Point mouse_0;
    std::string username;
    sf::TcpSocket socket;
    std::string text;
    char mode = 's';
    size_t received{};
    bool is_connected = false;

    //Private functions
    void initVariables();
    void initWindow();
    void initCursor();

public:
    //Public functions
    void initBoard();
    Game();
    static void initBase();
    void initBaseMenu();
    void initAnimal();
    void pushButtons();
    virtual ~Game();

    bool running() const;
    void pollEvents();
    void update();
    void render();
    void box ();
    void updateEnemy();
    void sendInfo();
    void receiveInfo();
    bool connect_to_server();
};

//Functions definitions:
void Game::initVariables() {
    this->window = nullptr;
}

void Game::initWindow() {
    this->videoMode.height = height;
    this->videoMode.width = width;
    this->window = new sf::RenderWindow(this->videoMode, "Chillness 1.1.3", sf::Style::Titlebar | sf::Style::Fullscreen);
    this->window->setFramerateLimit(40);

}

Game::~Game() {
    delete this->window;
}

Game::Game() {
    connect_to_server();
    initVariables();
    initWindow();
    initCursor();
    initBoard();
    initBase();
}

bool Game::running() const {
    return this->window->isOpen();
}

void Game::pollEvents() {
    bool were_selected = false;
    while (this->window->pollEvent(this->ev)){
        switch(this->ev.type){
            case sf::Event::Closed:
                this->window->close();
                break;
            case sf::Event::KeyPressed:
                if (this->ev.key.code == sf::Keyboard::Escape)
                    this->window->close();
                else if (this->ev.key.code == sf::Keyboard::Num1)
                    selected_type = 1;
                else if (this->ev.key.code == sf::Keyboard::Num2)
                    selected_type = 2;
                else if (this->ev.key.code == sf::Keyboard::Num3)
                    selected_type = 3;
                break;
            case sf::Event::MouseButtonPressed:
                if (this->ev.mouseButton.button == sf::Mouse::Left) {
                    for (int i = 0; i < simple_animals.size(); i++){
                        if (simple_animals[i]->is_selected()){
                            simple_animals[i]->select(false);
                        }
                    }
                    for (auto animal : simple_animals){
                        if (mouse.distance(animal->pos) < animal->size) animal->select(true);
                    }

                    mouse_0 = mouse;
                    area.setFillColor(sf::Color(200, 0, 100, 100));
                }
                if (this->ev.mouseButton.button == sf::Mouse::Right) {
                    for (int i = 0; i < simple_animals.size(); i++){
                        if (simple_animals[i]->is_selected()){
                            simple_animals[i]->set_aim(Point(mouse.get_x(), mouse.get_y()));
                            simple_animals[i]->stable = false;
                        }
                    }
                }
                break;

            case sf::Event::MouseButtonReleased:
                if (this->ev.mouseButton.button == sf::Mouse::Left) {
                    this->box();
                    for (int i = 0; i < simple_animals.size(); i++){
                        if (simple_animals[i]->is_selected()) were_selected = true;
                    }
                    if (!were_selected){
                        this->pushButtons();
                        this->initAnimal();
                        this->initBaseMenu();
                    }
                }
                break;
        }
    }
}

void Game::update() {
    if(is_connected) updateEnemy();
    mouse.set_x(sf::Mouse::getPosition(*this->window).x);
    mouse.set_y(sf::Mouse::getPosition(*this->window).y);
    this->pollEvents();
    area.setPosition(mouse_0.get_x(), mouse_0.get_y());
    float size_x = mouse.get_x() - mouse_0.get_x();
    float size_y = mouse.get_y() - mouse_0.get_y();
    area.setSize(sf::Vector2(size_x, size_y));

    for(auto & animal : simple_animals){
        for (auto bullet : bullets){
            if (bullet->pos.distance(animal->pos) < animal->size){
                bullet->hit(animal);
                if (animal->get_energy() < 0) delete animal;
            }
        }
        if(!animal->stable) animal->move();
        if(animal->pos.get_x() < animal->size) animal->set_pos(Point(animal->size*1.5, animal->pos.get_y()));
        if(animal->pos.get_x() > width - animal->size) animal->set_pos(Point(width - animal->size*1.5, animal->pos.get_y()));
        if(animal->pos.get_y() < animal->size) animal->set_pos(Point(animal->pos.get_x() ,animal->size*1.5));
        if(animal->pos.get_y() > height - animal->size) animal->set_pos(Point(animal->pos.get_x(), height - animal->size*1.5));
        for(auto & another_animal : simple_animals) {
            double dist = animal->pos.distance(another_animal->pos);
            if (dist < animal->size * 2 and dist != 0) {
                Point pos = animal->pos;
                Point another_pos = another_animal->pos;
                animal->pos.set_x(pos.get_x() - animal->get_speed() * pos.delta_x(another_pos) / pos.distance(another_pos));
                animal->pos.set_y(pos.get_y() - animal->get_speed() * pos.delta_y(another_pos) / pos.distance(another_pos));
                if(!animal->stable) {
                    another_animal->pos.set_x(
                            another_pos.get_x() - animal->get_speed() * another_pos.delta_x(pos) / another_pos.distance(pos));
                    another_animal->pos.set_y(
                            another_pos.get_y() - animal->get_speed() * another_pos.delta_y(pos) / another_pos.distance(pos));
                }
            }
        }
        for(auto & another_animal : enemy_animals) {
            for (auto bullet : bullets){
                if (bullet->pos.distance(another_animal->pos) < another_animal->size){
                    bullet->hit(another_animal);
                    if (another_animal->get_energy() < 0) delete another_animal;
                }
            }
            double dist = animal->pos.distance(another_animal->pos);
            if (dist < animal->size * 2 and dist != 0) {
                Point pos = animal->pos;
                Point another_pos = another_animal->pos;
                animal->pos.set_x(pos.get_x() - animal->get_speed() * pos.delta_x(another_pos) / pos.distance(another_pos));
                animal->pos.set_y(pos.get_y() - animal->get_speed() * pos.delta_y(another_pos) / pos.distance(another_pos));
                if(!animal->stable) {
                    another_animal->pos.set_x(
                            another_pos.get_x() - animal->get_speed() * another_pos.delta_x(pos) / another_pos.distance(pos));
                    another_animal->pos.set_y(
                            another_pos.get_y() - animal->get_speed() * another_pos.delta_y(pos) / another_pos.distance(pos));
                }
            }
        }
    }

    if (mouse.get_x() >= 0 and mouse.get_y() >= 0 and mouse.get_x() <= this->videoMode.width and mouse.get_y() <= this->videoMode.height){
        this->cursor.setFillColor(sf::Color::Red);
        this->cursor.setPosition(mouse.get_x(), mouse.get_y());
        this->board.energy_lvl.setSize(sf::Vector2(energy*10.f, (height/30)*1.f));
    }
    else this->cursor.setFillColor(sf::Color::Green);

    if (board.spawn_base.pushed == 1) board.spawn_base.picture.setFillColor(sf::Color::Red);
    else board.spawn_base.picture.setFillColor(sf::Color::Green);

}

void Game::render() {
    window->clear(sf::Color(5, 0, 90, 255));
    window->draw(this->board.board);
    window->draw(this->board.energy_lvl_back);
    window->draw(this->board.energy_lvl);
    window->draw(this->board.energy_lvl_caption);
    window->draw(this->board.spawn_base.picture);
    window->draw(this->cursor);


    for(auto & base : bases){
        this->window->draw(base.picture);
        if (base.is_selected){
            this->window->draw(base.menu.picture);
        }
    }
    for(auto & animal : simple_animals) {
        if (animal->is_selected()) animal->picture.setFillColor(red);
        else animal->picture.setFillColor(green);
        animal->picture.setPosition(animal->pos.get_x(), animal->pos.get_y());
        window->draw(animal->picture);
    }
    for(auto & animal : enemy_animals) {
        window->draw(animal->picture);
    }

    window->draw(this->area);
    this->window->display();
}

void Game::initCursor() {
    cursor.setSize(sf::Vector2(20.f, 20.f));
    cursor.setFillColor(sf::Color::Magenta);
    cursor.setOutlineColor(sf::Color::Green);
    cursor.setOutlineThickness(1.f);
    cursor.setOrigin(sf::Vector2(5.f, 5.f));

    area.setPosition(sf::Vector2(1.f, 1.f));
    area.setOrigin(sf::Vector2(1.f, 1.f));
    area.setSize(sf::Vector2(2.f, 2.f));
    area.setFillColor(sf::Color(0, 0, 0, 0));
}

void Game::initBoard() {
    board.energy_lvl.setPosition(0, height*0.95);
    board.energy_lvl.setSize(sf::Vector2((width/10)*1.f,  (height/30)*1.f));
    board.energy_lvl.setFillColor(sf::Color::Blue);

    board.energy_lvl_caption.setCharacterSize(height/30);
    board.energy_lvl_caption.setFont(font);
    board.energy_lvl_caption.setPosition(width*0.01, height*0.94);
    board.energy_lvl_caption.setFillColor(sf::Color (250, 200, 200));
    board.energy_lvl_caption.setString("energy");

    board.energy_lvl_back.setPosition(0, 0.95*height);
    board.energy_lvl_back.setSize(sf::Vector2((width/3)*1.f, (height/30)*1.f));
    board.energy_lvl_back.setFillColor(sf::Color::Black);

    board.board.setPosition(0, height*0.93);
    board.board.setSize(sf::Vector2(width*1.f, (height/15)*1.f));
    board.board.setFillColor(sf::Color::White);

    //this->board.spawn_base.picture.setPosition(1725, 1445);
    //this->board.spawn_base.picture.setSize(sf::Vector2(50.f, 50.f));
    //this->board.spawn_base.picture.setFillColor(sf::Color::Green);
    //this->board.spawn_base.picture.setOrigin(25,25);
}

void Game::initBase() {
    for(int i = 0; i < bases.size(); i++) {
        bases[i].picture.setOrigin(base_size, base_size);
        bases[i].picture.setPosition(bases[i].pos.get_x(), bases[i].pos.get_y());
        bases[i].picture.setRadius(bases[i].radius);
        bases[i].picture.setFillColor(bases[i].color);
    }
}

void Game::initBaseMenu(){
    BaseMenu menu_ = BaseMenu();
    Point a = mouse;
    for (int i = 0; i < 7; i++){
        bases[i].is_selected = false;
        if (bases[i].is_in_base(a)){
            menu_.picture.setFillColor(sf::Color(255, 255, 255, 150));
            menu_.picture.setSize(sf::Vector2f(width / 4, height / 4));
            menu_.picture.setOutlineColor(sf::Color::Red);
            menu_.picture.setOutlineThickness(5);
            menu_.picture.setPosition(width * 3 / 4, height * 3 / 4);
            bases[i].menu = menu_;
            bases[i].is_selected = true;
        }
    }
}

void Game::initAnimal() {
    bool spawned = false;
    int counter = 0;
    while (!spawned and counter < 10) {
        counter++;
        int value_x = 0 - base_size + (rand() / ((RAND_MAX + 1u) / (base_size * 2)));
        int value_y = 0 - base_size + (rand() / ((RAND_MAX + 1u) / (base_size * 2)));
        bool near_animal = false;
        bool near_base = false;
        Point position(0, 0);
        position.set_x(mouse.get_x());
        position.set_y(mouse.get_y());
        int i_spawn;
        for (int i = 0; i < bases.size(); i++) {
            if (position.distance(bases[i].pos) < base_size) {
                near_base = true;
                i_spawn = i;
                position = bases[i_spawn].pos;
                position.set_x(position.get_x() + value_x);
                position.set_y(position.get_y() + value_y);
            }
        }

        if(near_base){
            for (auto &animal : simple_animals) {
                if (position.distance(animal->get_pos()) < animal->size * 2) near_animal = true;
                }

            if (!near_animal and energy >= price_of_animal[selected_type] and position.get_y() < height and
                position.get_x() < width) {
                energy -= price_of_animal[selected_type];
                Point aim_ = position;
                switch (selected_type) {
                    case 1: {
                        auto beast = new Simple_Animal(100, 10, (int)(width / 192), aim_, position);
                        beast->draw();
                        simple_animals.push_back(beast);
                        break;
                    }
                    case 2: {
                        auto beast = new Shouter_Animal(100, 10, (int)(width / 192), aim_, position);
                        beast->draw();
                        simple_animals.push_back(beast);
                        break;
                    }
                }
                spawned = true;
                }
            }
        }
    }

void Game::pushButtons() {
        if (mouse.get_x() >= 0 and mouse.get_y() >= 0 and mouse.get_x() <= this->videoMode.width and mouse.get_y() <= this->videoMode.height
            and abs(mouse.get_x() - board.spawn_base.picture.getPosition().x) < board.spawn_base.picture.getSize().x/2 and
            abs(mouse.get_y() - board.spawn_base.picture.getPosition().y) < board.spawn_base.picture.getSize().y/2){
            this->board.spawn_base.pushed = !this->board.spawn_base.pushed;
        }
    }

void Game::box (){
    Point a1 = mouse;
    Point a2 = mouse_0;
    double xmax = std::max(a1.get_x(), a2.get_x());
    double xmin = std::min(a1.get_x(), a2.get_x());
    double ymax = std::max(a1.get_y(), a2.get_y());
    double ymin = std::min(a1.get_y(), a2.get_y());

    for (unsigned int i = 0; i < simple_animals.size(); i++){
        if ((simple_animals[i]->pos.get_x() < xmax && simple_animals[i]->pos.get_x() > xmin)
        && (simple_animals[i]->pos.get_y() < ymax && simple_animals[i]->pos.get_y() > ymin)){
            simple_animals[i]->select(true);
            simple_animals[i]->picture.setFillColor(red);
        }
    }
    mouse_0.set_x(0);
    mouse_0.set_y(0);
    area.setFillColor(sf::Color(0, 0, 0, 0));
}

void Game::updateEnemy() {
    enemy_animals.clear();
    sendInfo();
    receiveInfo();
}

void Game::sendInfo() {
    text = std::to_string(simple_animals.size());
    text += '_';
    for(int i = 0; i < simple_animals.size(); i++){
        text += std::to_string(simple_animals[i]->pos.get_x());
        text += '_';
        text += std::to_string(simple_animals[i]->pos.get_y());
        text += '_';
        text += std::to_string(simple_animals[i]->get_type());
        text += '_';
    }
    socket.send(text.c_str(), text.length() + 1);
}

void Game::receiveInfo() {
    char buffer[2000];
    socket.receive(buffer, sizeof(buffer), received);
    int number_of_enemies;
    std::string b = "";
    int k = 0;
    while (buffer[k] != '_'){
        b += buffer[k];
        k++;
    }
    number_of_enemies = std::stoi(b);

    for(int i = 0; i < number_of_enemies; i++){
        Point pos;
        int type = 0;
        k++;
        b = "";
        while (buffer[k] != '_'){
            b += buffer[k];
            k++;
        }
        pos.set_x(std::stoi(b));
        k++;
        b = "";
        while (buffer[k] != '_'){
            b += buffer[k];
            k++;
        }
        pos.set_y(std::stoi(b));
        k++;
        b = "";
        while (buffer[k] != '_'){
            b += buffer[k];
            k++;
        }
        type = std::stoi(b);
        switch (type) {
            case 1:{
                Simple_Animal enemy = Simple_Animal(energy, 100, 5, pos, pos);
                enemy.color = sf::Color::Cyan;
                enemy.draw();
                enemy_animals.push_back(&enemy);
            }
            case 2:{
                Shouter_Animal enemy = Shouter_Animal(energy, 100, 5, pos, pos);
                enemy.color = sf::Color::Cyan;
                enemy.draw();
                enemy_animals.push_back(&enemy);
            }
        }
    }

}

bool Game::connect_to_server() {
    char buffer[2000];
    username = "Egor";
    sf::IpAddress ip = "192.168.0.103";
    sf::TcpSocket::Status connection = socket.connect(ip, 2000);
    if(connection == sf::Socket::Done) {
        is_connected = true;
        std::cout << ip << endl;
        socket.receive(buffer, sizeof(buffer), received);
        socket.send(username.c_str(), username.length() + 1);
        std::cout << buffer << endl;
        std::cout << "Waiting for the second player..." << endl;
        socket.receive(buffer, sizeof(buffer), received);
        std::cout << buffer << endl;
        std::cout << "Game will be started soon..." << endl;
    } else std::cout << "You are offline" << endl;
    return is_connected;
}

//------------------------------------------------------GAME LOOP-------------------------------------------------------
    int main() {
        font.loadFromFile("/home/egor/Repositories/Chillness/20443.otf");
//Init Game
        Game game;
//Game loop
        while(game.running())
        {
//Update
            game.update();
//Render
            game.render();
        }
        return 0;
    }
