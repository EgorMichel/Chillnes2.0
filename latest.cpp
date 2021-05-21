#include "Chillness_Classes.cpp"

//Game class--------------------------------------------------------
class Game
{
private:
    //Private variables
    int teamType;
    float velocity = 0.05;
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
    bool is_connected = false;
    bool isInMenu = false;
    sf::RectangleShape goToMM;
    sf::Text goToMMCaption;

    //Private functions
    void initVariables();
    void initWindow();
    void initCursor();

public:
    bool readyToStart = false;
    sf::RenderWindow* window{};
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
    window = nullptr;

}

void Game::initWindow() {

    this->videoMode.height = height;
    this->videoMode.width = width;
    this->window = new sf::RenderWindow(this->videoMode, "Chillness", sf::Style::Titlebar | sf::Style::Fullscreen);
    this->window->setFramerateLimit(40);

}

Game::~Game() {
    delete window;
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
    if(not isInMenu) {
        bool were_selected = false;
        while (this->window->pollEvent(ev)) {
            switch (ev.type) {
                case sf::Event::Closed:
                    window->close();
                    break;
                case sf::Event::KeyPressed:
                    if (ev.key.code == sf::Keyboard::Escape)
                        this->window->close();
                    else if (ev.key.code == sf::Keyboard::Num1)
                        selected_type = 1;
                    else if (ev.key.code == sf::Keyboard::Num2)
                        selected_type = 2;
                    else if (ev.key.code == sf::Keyboard::Num3)
                        selected_type = 3;
                    else if (ev.key.code == sf::Keyboard::Space)
                        isInMenu = !isInMenu;
                    else if (ev.key.code == sf::Keyboard::S)
                        for (auto animal : simple_animals){
                            if (animal->is_selected())
                                animal->set_aim(animal->pos);
                        }

                    break;

                case sf::Event::MouseButtonPressed:
                    if (ev.mouseButton.button == sf::Mouse::Left) {

                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
                            int current_type = 0;
                            for (auto animal : simple_animals) {
                                if (mouse.distance(animal->pos) < animal->size) {
                                    current_type = animal->get_type();
                                }
                                animal->select(false);
                            }
                            for (auto animal : simple_animals) {
                                if (animal->get_type() == current_type) {
                                    animal->select(true);
                                }
                            }
                        }
                        else{
                            for (auto animal : simple_animals) {
                                if (animal->is_selected() and not (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))) {
                                    animal->select(false);
                                }
                                if (mouse.distance(animal->pos) < animal->size) {
                                    animal->select(true);
                                }
                            }
                        }


                        mouse_0 = mouse;
                        area.setFillColor(sf::Color(200, 0, 100, 100));

                        if(std::abs(mouse.get_x() - board.upgrade_1.picture.getPosition().x) < board.upgrade_1.picture.getSize().x/2 and
                           std::abs(mouse.get_y() - board.upgrade_1.picture.getPosition().y) < board.upgrade_1.picture.getSize().y/2){
                            if( energy > 30 * velocity * 8) {
                                velocity += 0.05;
                                energy -= 30 * velocity * 8;
                            }
                        }
                    }
                    if (ev.mouseButton.button == sf::Mouse::Right) {

                        for (auto animal : simple_animals) {
                            if (animal->is_selected()) {
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                                    std::cout<<"sosi"<<'\n';
                                    animal->aims.push_back(Point(mouse.get_x(), mouse.get_y()));
                                    animal->stable = false;
                                }
                                else{
                                    animal->set_aim(Point(mouse.get_x(), mouse.get_y()));
                                    animal->stable = false;
                                }
                            }
                        }
                    }
                    break;

                case sf::Event::MouseButtonReleased:
                    if (this->ev.mouseButton.button == sf::Mouse::Left) {
                        this->box();
                        for (auto & simple_animal : simple_animals) {
                            if (simple_animal->is_selected()) were_selected = true;
                        }
                        if (!were_selected) {
                            this->initAnimal();
                            this->initBaseMenu();
                        }
                    }
                    break;
            }
        }
    } else {
        while (this->window->pollEvent(this->ev)) {
            switch (this->ev.type) {
                case sf::Event::MouseButtonPressed:
                    if (this->ev.mouseButton.button == sf::Mouse::Left) {
                        if (std::abs(mouse.get_x() - goToMM.getPosition().x) < goToMM.getSize().x / 2 and
                            std::abs(mouse.get_y() - goToMM.getPosition().y) < goToMM.getSize().y / 2) {
                            window->close();
                        }
                    }
                    break;

                case sf::Event::Closed:
                    this->window->close();
                    break;
                case sf::Event::KeyPressed:
                    if (this->ev.key.code == sf::Keyboard::Space)
                        isInMenu = !isInMenu;
                    break;
            }
        }
    }
}

void Game::update() {

    mouse.set_x(sf::Mouse::getPosition(*this->window).x);
    mouse.set_y(sf::Mouse::getPosition(*this->window).y);
    this->pollEvents();

    if(not isInMenu) {

        int number_of_our_bases = 0;
        for(int i = 0; i < bases.size(); i++){
            if (bases[i].teamType == teamType){
                number_of_our_bases++;
            }
        }

        if(energy <= 100 - velocity) energy += velocity + number_of_our_bases * 0.02;
        board.chosen_type.setPosition(width * (5 + selected_type) / 15, height * 29 / 30);

        if (is_connected) updateEnemy();

        this->pollEvents();
        area.setPosition(mouse_0.get_x(), mouse_0.get_y());
        float size_x = mouse.get_x() - mouse_0.get_x();
        float size_y = mouse.get_y() - mouse_0.get_y();
        area.setSize(sf::Vector2(size_x, size_y));

        for (auto &animal : simple_animals) {
            animal->tact_counter += 1;
            animal->tact_counter2 += 1;
            for (auto bullet : enemy_bullets) {
                if (bullet->pos.distance(animal->pos) < animal->size + bullet->size) {
                    bullet->hit(animal);
                }
            }
            if (animal->get_energy() < 0){
                auto iterator = std::find(simple_animals.begin(), simple_animals.end(), animal);
                simple_animals.erase(iterator);
            }
            if (!animal->stable) animal->move();
            animal->attack();
            animal->get_damage();
            if (animal->pos.get_x() < animal->size) animal->set_pos(Point(animal->size * 1.5, animal->pos.get_y()));
            if (animal->pos.get_x() > width - animal->size)
                animal->set_pos(Point(width - animal->size * 1.5, animal->pos.get_y()));
            if (animal->pos.get_y() < animal->size) animal->set_pos(Point(animal->pos.get_x(), animal->size * 1.5));
            if (animal->pos.get_y() > height - animal->size)
                animal->set_pos(Point(animal->pos.get_x(), height - animal->size * 1.5));
            for (auto &another_animal : simple_animals) {
                double dist = animal->pos.distance(another_animal->pos);
                if (dist < animal->size * 2 and dist != 0) {
                    Point pos = animal->pos;
                    Point another_pos = another_animal->pos;
                    animal->pos.set_x(
                            pos.get_x() - animal->get_speed() * pos.delta_x(another_pos) / pos.distance(another_pos));
                    animal->pos.set_y(
                            pos.get_y() - animal->get_speed() * pos.delta_y(another_pos) / pos.distance(another_pos));
                    if (!animal->stable) {
                        another_animal->pos.set_x(
                                another_pos.get_x() -
                                animal->get_speed() * another_pos.delta_x(pos) / another_pos.distance(pos));
                        another_animal->pos.set_y(
                                another_pos.get_y() -
                                animal->get_speed() * another_pos.delta_y(pos) / another_pos.distance(pos));
                    }
                }
            }
            for (auto &another_animal : enemy_animals) {
                double dist = animal->pos.distance(another_animal->pos);
                if (dist < animal->size * 2 and dist != 0) {
                    Point pos = animal->pos;
                    Point another_pos = another_animal->pos;
                    animal->pos.set_x(
                            pos.get_x() - animal->get_speed() * pos.delta_x(another_pos) / pos.distance(another_pos));
                    animal->pos.set_y(
                            pos.get_y() - animal->get_speed() * pos.delta_y(another_pos) / pos.distance(another_pos));
                    if (!animal->stable) {
                        another_animal->pos.set_x(
                                another_pos.get_x() -
                                animal->get_speed() * another_pos.delta_x(pos) / another_pos.distance(pos));
                        another_animal->pos.set_y(
                                another_pos.get_y() -
                                animal->get_speed() * another_pos.delta_y(pos) / another_pos.distance(pos));
                    }
                }
            }
        }

        for(auto & base : bases){
            bool capturing = false;
            if(teamType != base.teamType) {
                for (auto &animal : simple_animals) {
                    if (base.is_in_base(animal->pos)) capturing = true;
                }
                for (auto &animal : enemy_animals) {
                    if (base.is_in_base(animal->pos)) capturing = false;
                }
            }
            if(!capturing) base.counterOfCaption = 0;
            else if(capturing) base.counterOfCaption += 1;
            if(base.counterOfCaption >= 40) {
                base.counterOfCaption = 0;
                base.teamType = teamType;
            }

        }

        for (auto i = 0; i < bullets.size(); i++){
            bullets[i]->move();
            bullets[i]->tact_counter += 1;
            if (bullets[i]->tact_counter > bullets[i]->lifetime) {
                bullets.erase(bullets.begin() + i);
                continue;
            }
            for (auto enemy_animal : enemy_animals){
                if (bullets[i]->pos.distance(enemy_animal->pos) < bullets[i]->size + enemy_animal->size - int(width/192)){
                    bullets.erase(bullets.begin() + i);
                    break;
                }
            }
        }


        if (mouse.get_x() >= 0 and mouse.get_y() >= 0 and mouse.get_x() <= this->videoMode.width and
            mouse.get_y() <= this->videoMode.height) {
            this->cursor.setFillColor(sf::Color::Red);
            this->cursor.setPosition(mouse.get_x(), mouse.get_y());
            this->board.energy_lvl.setSize(sf::Vector2(float(energy * width/300), (height / 30)));
        } else this->cursor.setFillColor(sf::Color::Green);


    } else{

    }
}

void Game::render() {
    if(not isInMenu) {
        const sf::Texture * t = &background_texture;
        window->clear(sf::Color(5, 0, 90, 255));
        sf::RectangleShape back;
        back.setSize(sf::Vector2(width, height));
        back.setPosition(0,0);
        back.setTexture(t);
        window->draw(back);
        window->draw(board.board);
        window->draw(board.energy_lvl_back);
        window->draw(board.energy_lvl);
        window->draw(board.energy_lvl_caption);
        window->draw(board.spawn_base.picture);
        window->draw(board.chosen_type);
        window->draw(board.type_1);
        window->draw(board.type_2);
        window->draw(board.type_3);
        window->draw(cursor);
        window->draw(board.upgrade_1.picture);
        window->draw(board.upgrade_2.picture);
        window->draw(board.upgrade_1.caption);
        window->draw(board.upgrade_2.caption);


        for (auto &base : bases) {
            if(base.teamType == 0) base.picture.setFillColor(sf::Color(150, 150, 150));
            else if (base.teamType == 1) {
                texture.setRepeated(false);
                base.picture.setFillColor(sf::Color(255, 255, 255));
                const sf::Texture * t = &texture;
                base.picture.setTexture(t);
            }
            else if(base.teamType == -1){
                base_1_texture.setRepeated(false);
                base.picture.setFillColor(sf::Color(255, 255, 255));
                const sf::Texture * t = &enemy_texture;
                base.picture.setTexture(t);
            }
            window->draw(base.picture);
            if (base.is_selected) {
                window->draw(base.menu.picture);
                base.menu.caption.setCharacterSize(height / 30);
                base.menu.caption.setFont(font);
                base.menu.caption.setPosition(width * 5 / 6, height * 18 / 19);
                base.menu.caption.setFillColor(red);
                base.menu.caption.setString("lvl " + std::to_string(base.power));
                window->draw(base.menu.caption);
            }
        }
        for (auto &animal : simple_animals) {
            if (animal->get_type() == 1) animal->picture.setFillColor(sf::Color(200, 150, 100, (100 + animal->get_energy()*155/100)));
            if (animal->get_type() == 2) animal->picture.setFillColor(sf::Color(100, 130, 0, (100 + animal->get_energy()*155/100)));
            if (animal->get_type() == 3) animal->picture.setFillColor(sf::Color(0, 255, 0, (100 + animal->get_energy()*155/100)));
            animal->picture.setOutlineThickness(4);
            if (animal->is_selected()) animal->picture.setOutlineColor(sf::Color(255, 0, 0));
            else animal->picture.setOutlineColor(sf::Color(255, 255, 255, 0));
            animal->picture.setPosition(animal->pos.get_x(), animal->pos.get_y());
            window->draw(animal->picture);
        }
        for (auto &animal : enemy_animals) {
            window->draw(animal->picture);
        }
        for (auto bullet : bullets){
            bullet->picture.setPosition(bullet->pos.get_x(), bullet->pos.get_y());
            window->draw(bullet->picture);
        }
        for (auto bullet : enemy_bullets){
            window->draw(bullet->picture);
        }

        window->draw(this->area);
    } else{
        window->clear(sf::Color(60, 0, 90, 255));
        window->draw(goToMM);
        window->draw(goToMMCaption);
    }
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
    goToMM.setSize(sf::Vector2(width/4, height/10));
    goToMM.setOrigin(sf::Vector2(width/8, height/19));
    goToMM.setPosition(width/2, height/3);

    goToMMCaption.setCharacterSize(height / 30);
    goToMMCaption.setFont(font);
    goToMMCaption.setOrigin(width/15, 0);
    goToMMCaption.setPosition(width/2, height * 9 / 30);
    goToMMCaption.setFillColor(red);
    goToMMCaption.setString("main menu");

    board.energy_lvl.setPosition(0, height*0.95);
    board.energy_lvl.setSize(sf::Vector2((width/10)*1.f,  (height/30)*1.f));
    board.energy_lvl.setFillColor(sf::Color::Blue);

    board.energy_lvl_caption.setCharacterSize(height / 30);
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

    board.chosen_type.setSize(sf::Vector2(width/35, height/20));
    board.chosen_type.setOrigin(width/70, height/40);
    board.chosen_type.setFillColor(sf::Color(10, 0, 10, 0));
    board.chosen_type.setOutlineColor(green);
    board.chosen_type.setOutlineThickness(6);

    board.type_1.setRadius(width/120);
    board.type_1.setOrigin(width/120, width/120);
    board.type_1.setFillColor(green);
    board.type_1.setPosition(width * 6 / 15, height * 29 / 30);

    board.type_2.setRadius(width/100);
    board.type_2.setOrigin(width/100, width/100);
    board.type_2.setFillColor(green);
    board.type_2.setPosition(width * 7 / 15, height * 29 / 30);

    board.type_3.setRadius(width/100);
    board.type_3.setOrigin(width/100, width/100);
    board.type_3.setFillColor(green);
    board.type_3.setPosition(width * 8 / 15, height * 29 / 30);

    board.upgrade_1.picture.setSize(sf::Vector2(width/20, height/20));
    board.upgrade_1.picture.setOrigin(width/40, height/40);
    board.upgrade_1.picture.setFillColor(sf::Color(green));
    board.upgrade_1.picture.setOutlineColor(red);
    board.upgrade_1.picture.setOutlineThickness(3);
    board.upgrade_1.picture.setPosition(width * 9 / 15, height * 29 / 30);

    board.upgrade_1.caption.setCharacterSize(height / 60);
    board.upgrade_1.caption.setFont(font);
    board.upgrade_1.caption.setPosition(width * 0.576, height * 19 / 20);
    board.upgrade_1.caption.setFillColor(red);
    board.upgrade_1.caption.setString("speed \n   up");

    board.upgrade_2.picture.setSize(sf::Vector2(width/20, height/20));
    board.upgrade_2.picture.setOrigin(width/40, height/40);
    board.upgrade_2.picture.setFillColor(sf::Color(green));
    board.upgrade_2.picture.setOutlineColor(red);
    board.upgrade_2.picture.setOutlineThickness(3);
    board.upgrade_2.picture.setPosition(width * 10 / 15, height * 29 / 30);

    board.upgrade_2.caption.setCharacterSize(height / 60);
    board.upgrade_2.caption.setFont(font);
    board.upgrade_2.caption.setPosition(width * 0.64266, height * 19 / 20);
    board.upgrade_2.caption.setFillColor(red);
    board.upgrade_2.caption.setString("hp \nup");
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
        bases[i].picture.setOutlineThickness(0);
        if (bases[i].is_in_base(a)){
            bases[i].picture.setOutlineThickness(5);
            bases[i].picture.setOutlineColor(red);
            menu_.picture.setFillColor(sf::Color(255, 255, 255, 150));
            menu_.picture.setSize(sf::Vector2f(width / 4, height / 15));
            menu_.picture.setOutlineColor(sf::Color::Red);
            menu_.picture.setOutlineThickness(5);
            menu_.picture.setPosition(width * 3 / 4, height * 14 / 15);
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
            if (position.distance(bases[i].pos) < base_size and (bases[i].teamType == teamType or not is_connected)) {
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
                        sound.setBuffer(buf);
                        sound.setVolume(30);
                        sound.setPitch(3.f);
                        sound.play();
                        auto beast = new Simple_Animal(100, 10, (int)(width / 500), aim_, position);
                        beast->draw();
                        simple_animals.push_back(beast);
                        break;
                    }
                    case 2: {
                        sound.setBuffer(buf);
                        sound.setVolume(30);
                        sound.setPitch(2.f);
                        sound.play();
                        auto beast = new Shouter_Animal(100, 10, (int)(width / 500), aim_, position);
                        beast->draw();
                        simple_animals.push_back(beast);
                        break;
                    }
                    case 3: {
                        sound.setBuffer(buf);
                        sound.setVolume(30);
                        sound.setPitch(1.2f);
                        sound.play();
                        auto beast = new Healer_Animal(100, 10, (int)(width / 500), aim_, position);
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
    enemy_bullets.clear();
    enemy_animals.clear();
    sendInfo();
    receiveInfo();
}

void Game::sendInfo() {
    text = std::to_string(simple_animals.size());
    text += '_';
    for(int i = 0; i < simple_animals.size(); i++){
        text += std::to_string(int(10000 * simple_animals[i]->pos.get_x() / width));
        text += '_';
        text += std::to_string(int(10000 * simple_animals[i]->pos.get_y() / width));
        text += '_';
        text += std::to_string(simple_animals[i]->get_type());
        text += '_';
    }
    text += std::to_string(bullets.size());
    text += '_';
    for(int i = 0; i < bullets.size(); i++){
        text += std::to_string(int(10000 * bullets[i]->pos.get_x() / width));
        text += '_';
        text += std::to_string(int(10000 * bullets[i]->pos.get_y() / width));
        text += '_';
    }
    int number_of_our_bases = 0;
    vector<int> our_bases;
    for(int i = 0; i < bases.size(); i++){
        if (bases[i].teamType == teamType){
            our_bases.push_back(i);
            number_of_our_bases++;
        }
    }
    if (number_of_our_bases == 0) window->close();
    text += std::to_string(number_of_our_bases);
    text += '_';
    for(int i = 0; i < number_of_our_bases; i++){
        text += std::to_string(our_bases[i]);
        text += '_';
    }
    text += std::to_string(running());
    text += '_';


    socket.send(text.c_str(), text.length() + 1);
}

void Game::receiveInfo() {
    size_t received;
    char buffer[20000];
    socket.receive(buffer, sizeof(buffer), received);
    int number_of_enemies, number_of_bullets;
    int k = 0;
    number_of_enemies = stoi(read(k, buffer));
    for(int i = 0; i < number_of_enemies; i++){
        Point pos;
        int type;
        pos.set_x(stoi(read(k, buffer)) * width / 10000);
        pos.set_y(stoi(read(k, buffer)) * width / 10000);
        type = stoi(read(k, buffer));

        if (type == 1) {
            auto enemy = new Simple_Animal(100, 10, 5, pos, pos);
            enemy->ally = false;
            enemy->draw();
            enemy->picture.setFillColor(sf::Color(200, 100, 150));
            enemy_animals.push_back(enemy);

        }
        else if (type == 2) {
            auto enemy = new Shouter_Animal(100, 10, 5, pos, pos);
            enemy->ally = false;
            enemy->draw();
            enemy->picture.setFillColor(sf::Color(100, 0, 130));
            enemy_animals.push_back(enemy);
        }
        else if (type == 3) {
            auto enemy = new Healer_Animal(100, 10, 5, pos, pos);
            enemy->ally = false;
            enemy->draw();
            enemy->picture.setFillColor(sf::Color(255, 0, 255));
            enemy_animals.push_back(enemy);
        }
    }
    number_of_bullets = stoi(read(k, buffer));
    for(int i = 0; i < number_of_bullets; i++) {
        Point pos;
        pos.set_x(stoi(read(k, buffer)) * width / 10000);
        pos.set_y(stoi(read(k, buffer)) * width / 10000);
        auto b = new Bullet(int(width / 192), 10, 10, pos, pos);
        b->ally = false;
        b->draw();
        enemy_bullets.push_back(b);
    }
    int number_of_bases = stoi(read(k, buffer));
    for(int i = 0; i < number_of_bases; i++){
        bases[stoi(read(k, buffer))].teamType = -teamType;
    }
    if (stoi(read(k, buffer)) == 0) window->close();
}

bool Game::connect_to_server() {
    size_t received;
    char buffer[2000];
    username = "Egor";
    sf::IpAddress ip = "10.55.128.44"; //192.168.1.2  10.55.128.181  10.55.132.150 - Michel on miptng
    sf::TcpSocket::Status connection = socket.connect(ip, 2000);
    if(connection == sf::Socket::Done) {
        is_connected = true;

        std::cout << ip << endl;

        socket.send(username.c_str(), username.length() + 1);

        std::cout << "Waiting for the second player..." << endl;
        socket.receive(buffer, sizeof(buffer), received);
        teamType = std::stoi(buffer);
        if(teamType == 1) bases[0].teamType = 1;
        if(teamType == -1) bases[bases.size() - 1].teamType = -1;
        std::cout << "Game will be started soon..." << endl;
    } else std::cout << "You are offline" << endl;
    return is_connected;
}

class MainMenu
{
public:
    //Private variables
    sf::RenderWindow* window{};
    sf::VideoMode videoMode;
    sf::Event ev{};
    Point mouse;
    std::string choice = "0";

    sf::RectangleShape newGameButton;
    sf::Text newGameCaption;

    sf::RectangleShape aboutButton;
    sf::Text aboutButtonCaption;

    sf::RectangleShape exitButton;
    sf::Text exitCaption;

    sf::Text loadingCaption;
    bool loading = false;

    void pollEvents() {
        while (this->window->pollEvent(this->ev)) {
            switch (this->ev.type) {
                case sf::Event::Closed:
                    this->window->close();
                    break;
                case sf::Event::KeyPressed:
                    if (this->ev.key.code == sf::Keyboard::Escape)
                        this->window->close();
                    else if (this->ev.key.code == sf::Keyboard::Enter)
                        //choice = "start new game";
                        break;

                case sf::Event::MouseButtonPressed:
                    if (this->ev.mouseButton.button == sf::Mouse::Left) {
                        if(abs(mouse.get_x() - newGameButton.getPosition().x) < newGameButton.getSize().x/2 and
                           abs(mouse.get_y() - newGameButton.getPosition().y) < newGameButton.getSize().y/2){
                            choice = "start new game";
                        }
                        if(abs(mouse.get_x() - aboutButton.getPosition().x) < aboutButton.getSize().x/2 and
                           abs(mouse.get_y() - aboutButton.getPosition().y) < aboutButton.getSize().y/2){
                            choice = "about";
                        }
                        if(abs(mouse.get_x() - exitButton.getPosition().x) < exitButton.getSize().x/2 and
                           abs(mouse.get_y() - exitButton.getPosition().y) < exitButton.getSize().y/2){
                            choice = "exit";
                        }
                    }
                    break;

            }
        }
    }

    void update() {
        mouse.set_x(sf::Mouse::getPosition(*this->window).x);
        mouse.set_y(sf::Mouse::getPosition(*this->window).y);
        this->pollEvents();
    }

    void render(){
        window->clear(sf::Color(60, 100, 90, 255));
        window->draw(newGameButton);
        window->draw(newGameCaption);
        window->draw(aboutButton);
        window->draw(aboutButtonCaption);
        window->draw(exitButton);
        window->draw(exitCaption);
        if(loading) window->draw(loadingCaption);
        window->display();
    }

    MainMenu() {
        window = nullptr;
        videoMode.height = height;
        videoMode.width = width;
        window = new sf::RenderWindow(videoMode, "Chillness 1.1.3", sf::Style::Titlebar | sf::Style::Fullscreen);
        window->setFramerateLimit(40);

        newGameButton.setSize(sf::Vector2(width/4, height/10));
        newGameButton.setOrigin(sf::Vector2(width/8, height/19));
        newGameButton.setPosition(width/2, height/3);

        newGameCaption.setCharacterSize(height / 30);
        newGameCaption.setFont(font);
        newGameCaption.setOrigin(width/15, 0);
        newGameCaption.setPosition(width/2, height * 9 / 30);
        newGameCaption.setFillColor(red);
        newGameCaption.setString("new game");

        aboutButton.setSize(sf::Vector2(width/4, height/10));
        aboutButton.setOrigin(sf::Vector2(width/8, height/19));
        aboutButton.setPosition(width/2, height * 15 / 30);

        aboutButtonCaption.setCharacterSize(height / 30);
        aboutButtonCaption.setFont(font);
        aboutButtonCaption.setOrigin(width/13, 0);
        aboutButtonCaption.setPosition(width/2, height * 14 / 30);
        aboutButtonCaption.setFillColor(red);
        aboutButtonCaption.setString("about game");

        exitButton.setSize(sf::Vector2(width/4, height/10));
        exitButton.setOrigin(sf::Vector2(width/8, height/19));
        exitButton.setPosition(width/2, height * 20 / 30);

        exitCaption.setCharacterSize(height / 30);
        exitCaption.setFont(font);
        exitCaption.setOrigin(width/37, 0);
        exitCaption.setPosition(width/2, height * 19 / 30);
        exitCaption.setFillColor(red);
        exitCaption.setString("exit");

        loadingCaption.setCharacterSize(height / 30);
        loadingCaption.setFont(font);
        loadingCaption.setOrigin(width/37, 0);
        loadingCaption.setPosition(width/2, height * 24 / 30);
        loadingCaption.setFillColor(red);
        loadingCaption.setString("loading...");


    }
    ~MainMenu(){
        delete window;
    }
};

//------------------------------------------------------GAME LOOP-------------------------------------------------------
int main() {
    texture.loadFromFile("/home/egorchan/Chillnes2.0/p1.png");
    enemy_texture.loadFromFile("/home/egorchan/Chillnes2.0/p2.png");
    base_texture.loadFromFile("/home/egorchan/Chillnes2.0/round.jpg");
    base_1_texture.loadFromFile("/home/egorchan/Chillnes2.0/base.jpg");
    background_texture.loadFromFile("/home/egorchan/Chillnes2.0/background.jpg");




    sf::Music music;//создаем объект музыки
    music.openFromFile("/home/egorchan/Chillnes2.0/music.ogg");//загружаем файл
    music.setLoop(true);
    music.setVolume(15);
    music.play();

    buf.loadFromFile("/home/egorchan/Chillnes2.0/spawn.ogg");
    sf::Sound sound;
    sound.setBuffer(buf);

    buf_2.loadFromFile("/home/egorchan/Chillnes2.0/shoot.wav");
    sf::Sound sound_2;
    sound_2.setBuffer(buf_2);


    font.loadFromFile("/home/egor/Рабочий стол/Repositories/Chillnes2.0/font_1.ttf");
    while (true){
        MainMenu mm;
        mm.choice = "0";
        while (mm.choice == "0") {
            mm.update();
            mm.render();
        }

        //Init Game
        if (mm.choice == "start new game") {

            mm.loading = true;
            mm.window->close();
            Game * game = new Game();
            /*while (not game->readyToStart){
                cout << mm.loading;
                mm.render();
            }*/



            //Game loop
            while (game->running()) {
                //Update
                game->update();
                //Render
                game->render();
            }
            simple_animals.clear();
            enemy_animals.clear();
            bullets.clear();
            energy = 10;

            delete game;
        }
        else if(mm.choice == "exit") break;
    }
    return 0;
}
