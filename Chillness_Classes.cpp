//
// Created by egor on 19.05.2021.
//

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <ctime>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <string>


using std::vector;
using std::cout;
using std::endl;


//Global variables
float height = sf::VideoMode::getDesktopMode().height;
float width = sf::VideoMode::getDesktopMode().width;
float base_size = (float)width / 32;
double energy = 100;
vector<int> price_of_animal = {0, 5, 10, 30};
int selected_type = 1; //1 - simple, 2 - shouter
sf::Font font;
sf::Color green = sf::Color::Green;
sf::Color red = sf::Color::Red;
sf::Color white = sf::Color::White;


class Base;
class Animal;
class Bullet;

vector<Animal*> simple_animals = {};
vector<Animal*> enemy_animals = {};
vector<Bullet*> bullets = {};
vector<Bullet*> enemy_bullets = {};

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
        energy = energy_;
        strength = strength_;
        speed = speed_;
        aim = aim_;
        pos = pos_;
        color = green;
        selected = false;
        price = 0;
        size = width / 120;
        type = 0;
        time_flag = clock();
        tact_counter = 0;
    }
    virtual ~Animal() = default;
    void move() {
        if(pos.distance(aim) < this->size*2) stable = true;
        pos.set_x(floor(pos.get_x() + speed * pos.delta_x(aim) / (pos.distance(aim) + 1)));
        pos.set_y(floor(pos.get_y() + speed * pos.delta_y(aim) / (pos.distance(aim) + 1)));
    }
    virtual void draw() = 0;
    virtual void attack() = 0;
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
    long int time_flag;
    bool ally = true;
    int tact_counter = 0;
protected:
    int energy, strength, price, speed, type;
    bool selected;
};


class Bullet{
protected:
    int speed;
    int damage;
    int lifetime = 0;
    long time_flag = 0;
    sf::Color color = sf::Color(0, 0, 0);
    double cosinus;
    double sinus;
public:
    Bullet(int speed_, int damage_, int lifetime_, Point pos_, Point aim_){
        speed = speed_;
        damage = damage_;
        lifetime = lifetime_;
        time_flag = clock();
        pos = pos_;
        aim = aim_;
        cosinus = pos.delta_x(aim) / pos.distance(aim);
        sinus = pos.delta_y(aim) / pos.distance(aim);
    }
    void move();
    void draw();
    ~Bullet() = default;
    void hit(Animal* animal);
    Point pos;
    Point aim;
    sf::CircleShape picture;
    int size = width / 192;
    bool ally = true;
    int tact_counter = 0;
};

void Bullet::draw(){
    if (ally)
        color.a = 255 - 155 * tact_counter / 40;  // may be laggy with higher energy values
    this->picture.setRadius(size);
    this->picture.setPosition(pos.get_x(), pos.get_y());
    this->picture.setOrigin(size, size);
    this->picture.setFillColor(this->color);
    this->picture.setOutlineThickness(0);
    this->picture.setOutlineColor(white);
}

void Bullet::move(){
    pos.set_x(pos.get_x() + speed * cosinus);
    pos.set_y(pos.get_y() + speed * sinus);
    if (tact_counter > lifetime and ally) {
        auto iterator = std::find(bullets.begin(), bullets.end(), this);
        bullets.erase(iterator);
    }
}

void Bullet::hit(Animal* animal){
    animal->set_energy(animal->get_energy() - damage);
}

//Animal types:

//Simple_Animal

class Simple_Animal: public Animal
{
public:
    Simple_Animal(int energy_, int strength_, int speed_, Point aim_, Point pos_):
            Animal(energy_, strength_, speed_, aim_, pos_){
        price = 15;
        strength = strength_;
        speed = speed_;
        size = width / 120;
        type = 1;
    }
    void attack() final;
    void capture(Base* base) final;
    void draw() final;
};

void Simple_Animal::draw() {
//    if (ally)
//        color.a = 150 + energy;  // may be laggy with higher energy value
//    else color.a = 255;
    picture.setRadius(this->size);
    picture.setPosition(this->get_pos().get_x(), this->get_pos().get_y());
    picture.setOrigin(this->size, this->size);
    picture.setFillColor(this->color);
    picture.setOutlineThickness(0);
    picture.setOutlineColor(white);
}

void Simple_Animal::attack(){
    if (tact_counter < 20) return;
    for (auto opponent : enemy_animals)
        if (pos.distance(opponent->pos) < 1.5 * (size + opponent->size)){
            energy -= opponent->get_strength();
        }
    tact_counter = 0;
}

void Simple_Animal::capture(Base* base){
}


//Shouter_Animal
class Shouter_Animal: public Animal{
public:
    Shouter_Animal(int energy_, int strength_, int speed_, Point aim_, Point pos_):
            Animal(energy_, strength_, speed_, aim_, pos_){
        price = 30;
        strength = strength_;
        speed = speed_;
        size = width / 100;
        type = 2;
    }
    void attack() final;
    void capture(Base* base) final;
    void draw() final;
};

void Shouter_Animal::draw() {
//    if (ally)
//        color.a = 150 + energy;  // may be laggy with higher energy values
    this->picture.setRadius(this->size);
    this->picture.setPosition(this->get_pos().get_x(), this->get_pos().get_y());
    this->picture.setOrigin(this->size, this->size);
    this->picture.setFillColor(this->color);
    this->picture.setOutlineThickness(0);
    this->picture.setOutlineColor(white);
}

void Shouter_Animal::attack() {
    if (tact_counter < 40) return;
    if (!stable) return;
    if (not enemy_animals.empty()){
        double shortest_distance = 999999;
        int index = 0;
        for (int i = 0; i < enemy_animals.size(); i++){
            double temp = pos.distance(enemy_animals[i]->pos);
            if (temp < shortest_distance){
                shortest_distance = temp;
                index = i;
            }
        }
        Point aim_ = enemy_animals[index]->pos;
        auto bullet = new Bullet(int(width / 192), strength, 20, pos, aim_);
        bullets.push_back(bullet);
        bullet->draw();
        tact_counter = 0;
    }
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
    sf::Text caption;
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
    int power = 0;
    int teamType = 0;
    int counterOfCaption = 0;

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
    sf::RectangleShape chosen_type;
    sf::RectangleShape energy_lvl;
    sf::RectangleShape energy_lvl_back;
    sf::RectangleShape board;
    button spawn_base;
    sf::Text energy_lvl_caption;
    sf::CircleShape type_1;
    sf::CircleShape type_2;
    button upgrade_1;
    button upgrade_2;
};

std :: string read(int &k, char* buffer){
    std:: string b = "";
    while (buffer[k] != '_'){
        b += buffer[k];
        k++;
    }
    k++;
    return b;
}