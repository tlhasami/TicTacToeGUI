#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

using namespace sf;
using namespace std;

// ----------------- Player Class -----------------
void drawString(RenderWindow &window, const string &text, float x, float y, unsigned int size = 30, Color color = Color::Black, const string &fontPath = "assets/font.ttf")
{
    static Font font;
    static bool loaded = false;
    if (!loaded)
    {
        if (!font.loadFromFile(fontPath))
        {
            cerr << "Failed to load font: " << fontPath << endl;
            return;
        }
        loaded = true;
    }
    Text sfText;
    sfText.setFont(font);
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(color);
    sfText.setPosition(x, y);
    window.draw(sfText);
}

class Player
{
public:
    string username;
    int wins;

    Player(const string &name = "", int s = 0) : username(name), wins(s) {}

    void saveToFile()
    {
        ofstream file("player_data.txt", ios::app);
        file << username << " " << wins << endl;
    }

    static Player loadFromFile(const string &username)
    {
        ifstream file("player_data.txt");
        string name;
        int wins;
        while (file >> name >> wins)
        {
            if (name == username)
            {
                return Player(name, wins);
            }
        }
        return Player("", 0);
    }
    void updatePlayerScore(const string &username, int newScore)
    {
        ifstream inFile("player_data.txt");
        ofstream tempFile("temp.txt");

        string name;
        int wins;
        bool updated = false;

        while (inFile >> name >> wins)
        {
            if (name == username)
            {
                tempFile << name << " " << newScore << endl;
                updated = true;
            }
            else
            {
                tempFile << name << " " << wins << endl;
            }
        }

        // If user was not found, add as new
        if (!updated)
        {
            tempFile << username << " " << newScore << endl;
        }

        inFile.close();
        tempFile.close();

        // Replace original file with updated one
        remove("player_data.txt");
        rename("temp.txt", "player_data.txt");
    }
};

Player currentPlayer;
int player1Score = 0;
int player2Score = 0;

// ----------------- Utility: Check username -----------------
bool usernameExists(const string &name)
{
    ifstream file("user.txt");
    string line;
    while (getline(file, line))
    {
        if (line == name)
            return true;
    }
    return false;
}

void saveUsername(const string &name)
{
    ofstream file("user.txt", ios::app);
    file << name << endl;
}

// ----------------- Object Class -----------------
class Object
{
private:
    Texture texture;
    Sprite sprite;

public:
    bool load(const string &imagePath)
    {
        if (!texture.loadFromFile(imagePath))
        {
            cerr << "Failed to load image: " << imagePath << endl;
            return false;
        }
        sprite.setTexture(texture);
        return true;
    }

    void setPosition(float x, float y)
    {
        sprite.setPosition(x, y);
    }

    void draw(RenderWindow &window)
    {
        window.draw(sprite);
    }

    Sprite &getSprite()
    {
        return sprite;
    }
};

// ----------------- Button Class -----------------
class Button
{
private:
    RectangleShape rect;

public:
    Button(float x, float y, float width, float height, Color color = Color::White)
    {
        rect.setPosition(x, y);
        rect.setSize(Vector2f(width, height));
        rect.setFillColor(color);
    }

    void draw(RenderWindow &window)
    {
        window.draw(rect);
    }

    bool isClicked(Event &event, RenderWindow &window)
    {
        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
        {
            Vector2i mousePos = Mouse::getPosition(window);
            FloatRect bounds = rect.getGlobalBounds();
            return bounds.contains((float)mousePos.x, (float)mousePos.y);
        }
        return false;
    }

    void setColor(Color color)
    {
        rect.setFillColor(color);
    }

    void setPosition(float x, float y)
    {
        rect.setPosition(x, y);
    }

    Vector2f getPosition()
    {
        return rect.getPosition();
    }
};

// ----------------- Login Class -----------------
class Login
{
private:
    Object Background; // login.png
    Button Submit;
    Button Back;
    Text InputText;
    Text Availability;
    Font font;
    string inputUsername;
    bool isActive;

public:
    Login() : Background(),
              Submit(414, 490, 186, 65),
              Back(18, 543, 80, 80),
              isActive(false)
    {
        if (!Background.load("assets/login.png"))
        {
            cerr << "Failed to load login background image" << endl;
        }

        if (!font.loadFromFile("assets/font.ttf"))
        {
            cerr << "Failed to load font" << endl;
        }

        InputText.setString("");
        InputText.setFont(font);
        InputText.setCharacterSize(35);
        InputText.setFillColor(Color::Black);
        InputText.setPosition(377, 290);

        Availability.setString("");
        Availability.setFont(font);
        Availability.setCharacterSize(25);
        Availability.setPosition(355, 400);
    }

    string usernameTaking(RenderWindow &window)
    {
        isActive = true;
        inputUsername.clear();
        InputText.setString("");
        Availability.setString("");

        while (isActive && window.isOpen())
        {
            Event event;

            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    window.close();
                    return "";
                }

                if (event.type == Event::TextEntered)
                {
                    if (event.text.unicode == '\b')
                    {
                        if (!inputUsername.empty())
                        {
                            inputUsername.pop_back();
                        }
                    }
                    else if (event.text.unicode < 128 &&
                             event.text.unicode != '\r' &&
                             event.text.unicode != '\t')
                    {
                        inputUsername += static_cast<char>(event.text.unicode);
                    }
                    InputText.setString(inputUsername);

                    if (inputUsername.empty())
                    {
                        Availability.setString("");
                    }
                    else if (!usernameExists(inputUsername))
                    {
                        Availability.setString("Username not found");
                        Availability.setFillColor(Color::Red);
                    }
                    else
                    {
                        Availability.setString("Username found");
                        Availability.setFillColor(Color(0, 120, 0));
                    }
                }

                if (Submit.isClicked(event, window))
                {
                    if (inputUsername.empty())
                    {
                        Availability.setString("Enter a username");
                        Availability.setFillColor(Color::Magenta);
                    }
                    else if (usernameExists(inputUsername))
                    {
                        isActive = false;
                        return inputUsername;
                    }
                }

                if (Back.isClicked(event, window))
                {
                    isActive = false;
                    return "";
                }
            }

            window.clear();
            Background.draw(window);

            window.draw(InputText);
            window.draw(Availability);
            window.display();
        }
        return "";
    }
};

// ----------------- SignIn Class -----------------
class SignIn
{
private:
    Object Background;
    Button Submit;
    Button Back;
    Text InputText;
    Text Availability;
    Font font;
    string inputUsername;
    bool isActive;

public:
    SignIn() : Background(),
               Submit(414, 490, 186, 65),
               Back(18, 543, 80, 80),
               isActive(false)
    {
        if (!Background.load("assets/signIn.png"))
        {
            cerr << "Failed to load signIn background image" << endl;
        }

        if (!font.loadFromFile("assets/font.ttf"))
        {
            cerr << "Failed to load font" << endl;
        }

        InputText.setString("");
        InputText.setFont(font);
        InputText.setCharacterSize(35);
        InputText.setFillColor(Color::Black);
        InputText.setPosition(377, 290);

        Availability.setString("");
        Availability.setFont(font);
        Availability.setCharacterSize(25);
        Availability.setPosition(355, 400);
    }

    string usernameTaking(RenderWindow &window)
    {
        isActive = true;
        inputUsername.clear();
        InputText.setString("");
        Availability.setString("");

        while (isActive && window.isOpen())
        {
            Event event;

            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    window.close();
                    return "";
                }

                if (event.type == Event::TextEntered)
                {
                    if (event.text.unicode == '\b')
                    {
                        if (!inputUsername.empty())
                        {
                            inputUsername.pop_back();
                        }
                    }
                    else if (event.text.unicode < 128 &&
                             event.text.unicode != '\r' &&
                             event.text.unicode != '\t')
                    {
                        inputUsername += static_cast<char>(event.text.unicode);
                    }
                    InputText.setString(inputUsername);

                    if (inputUsername.empty())
                    {
                        Availability.setString("");
                    }
                    else if (usernameExists(inputUsername))
                    {
                        Availability.setString("Username not available");
                        Availability.setFillColor(Color::Red);
                    }
                    else
                    {
                        Availability.setString("Username available");
                        Availability.setFillColor(Color(0, 120, 0));
                    }
                }

                if (Submit.isClicked(event, window))
                {
                    if (inputUsername.empty())
                    {
                        Availability.setString("Enter a username");
                        Availability.setFillColor(Color::Red);
                    }
                    else if (!usernameExists(inputUsername))
                    {
                        saveUsername(inputUsername);
                        Player newPlayer(inputUsername);
                        newPlayer.saveToFile();
                        isActive = false;
                        return inputUsername;
                    }
                }

                if (Back.isClicked(event, window))
                {
                    isActive = false;
                    return "";
                }
            }

            window.clear();
            Background.draw(window);

            window.draw(InputText);
            window.draw(Availability);
            window.display();
        }
        return "";
    }
};

// ----------------- Tic Tac Toe Game Implementation -----------------
class TicTacToe
{
private:
    vector<vector<int>> board; // 0 = empty, 1 = X, 2 = O

    int currentPlayer;
    bool gameOver;
    int winner;
    Clock gameClock;
    Time timeLimit;
    bool vsAI;
    bool tournamentMode;
    int tournamentGames;
    int gamesPlayed;
    int player1Wins;
    int player2Wins;
    int gridSize; // Add this line to store grid size

    void initializeBoard()
    {
        // Clear the existing board
        board.clear();

        // Manually create and initialize each row
        for (int i = 0; i < gridSize; i++)
        {
            vector<int> row;
            for (int j = 0; j < gridSize; j++)
            {
                row.push_back(0); // Manually set each element to 0
            }
            board.push_back(row);
        }

        currentPlayer = 1;
        gameOver = false;
        winner = 0;
        gameClock.restart();
    }

    bool checkWin(int player)
    {
        // Check rows
        for (int i = 0; i < gridSize; i++)
        {
            bool win = true;
            for (int j = 0; j < gridSize; j++)
            {
                if (board[i][j] != player)
                {
                    win = false;
                    break;
                }
            }
            if (win)
                return true;
        }

        // Check columns
        for (int j = 0; j < gridSize; j++)
        {
            bool win = true;
            for (int i = 0; i < gridSize; i++)
            {
                if (board[i][j] != player)
                {
                    win = false;
                    break;
                }
            }
            if (win)
                return true;
        }

        // Check diagonals
        bool diag1 = true, diag2 = true;
        for (int i = 0; i < gridSize; i++)
        {
            if (board[i][i] != player)
                diag1 = false;
            if (board[i][gridSize - 1 - i] != player)
                diag2 = false;
        }
        return diag1 || diag2;
    }

    bool checkDraw()
    {
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                if (board[i][j] == 0)
                    return false;
            }
        }
        return true;
    }

    void makeAIMove()
    {
        // Simple AI: first try to win, then block, then random
        // Check for winning move
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                if (board[i][j] == 0)
                {
                    board[i][j] = 2;
                    if (checkWin(2))
                    {
                        return;
                    }
                    board[i][j] = 0;
                }
            }
        }

        // Block opponent's winning move
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                if (board[i][j] == 0)
                {
                    board[i][j] = 1;
                    if (checkWin(1))
                    {
                        board[i][j] = 2;
                        return;
                    }
                    board[i][j] = 0;
                }
            }
        }

        // Take center if available
        if (gridSize % 2 == 1)
        {
            int center = gridSize / 2;
            if (board[center][center] == 0)
            {
                board[center][center] = 2;
                return;
            }
        }

        // Random move
        vector<pair<int, int>> available;
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                if (board[i][j] == 0)
                {
                    available.push_back({i, j});
                }
            }
        }
        if (!available.empty())
        {
            int index = rand() % available.size();
            board[available[index].first][available[index].second] = 2;
        }
    }

public:
    TicTacToe() : player1Wins(0), player2Wins(0) {}

    void startClassicMode(int size)
    {
        gridSize = size;
        vsAI = false;
        tournamentMode = false;
        initializeBoard();
    }

    void startAIMode(int size)
    {
        gridSize = size;
        vsAI = true;
        tournamentMode = false;
        initializeBoard();
    }

    int getGridSize() const { return gridSize; }

    void startTournamentMode(int size, int games)
    {
        gridSize = size;
        tournamentMode = true;
        tournamentGames = games;
        gamesPlayed = 0;
        player1Wins = 0;
        player2Wins = 0;
        initializeBoard();
    }

    void startSpeedMode(int size, Time limit)
    {
        gridSize = size;
        timeLimit = limit;
        vsAI = false;
        tournamentMode = false;
        initializeBoard();
    }

    void handleClick(int row, int col)
    {
        if (row < 0 || row >= gridSize || col < 0 || col >= gridSize)
            return;
        if (gameOver || board[row][col] != 0)
            return;

        board[row][col] = currentPlayer;

        if (checkWin(currentPlayer))
        {
            gameOver = true;
            winner = currentPlayer;
            if (currentPlayer == 1)
                player1Wins++;
            else
                player2Wins++;
            return;
        }

        if (checkDraw())
        {
            gameOver = true;
            winner = 0;
            return;
        }

        currentPlayer = currentPlayer == 1 ? 2 : 1;

        if (vsAI && currentPlayer == 2 && !gameOver)
        {
            makeAIMove();
            if (checkWin(2))
            {
                gameOver = true;
                winner = 2;
                player2Wins++;
            }
            else if (checkDraw())
            {
                gameOver = true;
                winner = 0;
            }
            currentPlayer = 1;
        }
    }

    void update()
    {
        if (tournamentMode && gameOver)
        {
            gamesPlayed++;
            if (gamesPlayed < tournamentGames &&
                player1Wins <= tournamentGames / 2 &&
                player2Wins <= tournamentGames / 2)
            {
                initializeBoard();
            }
        }
    }

    void draw(RenderWindow &window)
    {
        
        

         Object Background;
        
         
         if (!Background.load("assets/game.png"))
        {
            cerr << "Failed to load signIn background image" << endl;
        }
        Background.draw(window);
        // Constants for the centered board
        const float boardSize = 450.f;
        const float offsetX = (window.getSize().x - boardSize) / 2.f;
        const float offsetY = (window.getSize().y - boardSize) / 2.f;
        const float cellSize = boardSize / gridSize;

        // Draw board background
        RectangleShape boardBackground(Vector2f(boardSize, boardSize));
        boardBackground.setPosition(offsetX, offsetY);
        boardBackground.setFillColor(Color(240, 240, 240)); // Light gray background
        boardBackground.setOutlineThickness(3.f);
        boardBackground.setOutlineColor(Color(50, 50, 50));
        window.draw(boardBackground);

        // Draw grid lines (thicker and darker)
        RectangleShape line(Vector2f(3.f, boardSize));
        line.setFillColor(Color(80, 80, 80));

        for (int i = 1; i < gridSize; i++)
        {
            // Vertical lines
            line.setPosition(offsetX + i * cellSize - 1.f, offsetY); // -1 to center the line
            window.draw(line);
            // Horizontal lines
            line.setSize(Vector2f(boardSize, 3.f));
            line.setPosition(offsetX, offsetY + i * cellSize - 1.f);
            window.draw(line);
            line.setSize(Vector2f(3.f, boardSize));
        }

        // Draw X's and O's with better visuals
        const float piecePadding = cellSize * 0.15f;
        const float pieceThickness = cellSize * 0.1f;

        for (int row = 0; row < gridSize; row++)
        {
            for (int col = 0; col < gridSize; col++)
            {
                float centerX = offsetX + col * cellSize + cellSize / 2.f;
                float centerY = offsetY + row * cellSize + cellSize / 2.f;
                float pieceSize = cellSize / 2.f - piecePadding;

                if (board[row][col] == 1)
                { // X
                    // Draw two lines for X
                    RectangleShape xLine(Vector2f(pieceThickness, pieceSize * 2.f));
                    xLine.setOrigin(pieceThickness / 2.f, pieceSize);
                    xLine.setPosition(centerX, centerY);
                    xLine.setFillColor(Color(200, 50, 50)); // Red X

                    // First diagonal
                    xLine.setRotation(45.f);
                    window.draw(xLine);

                    // Second diagonal
                    xLine.setRotation(-45.f);
                    window.draw(xLine);
                }
                else if (board[row][col] == 2)
                { // O
                    // Draw circle for O
                    CircleShape oPiece(pieceSize);
                    oPiece.setOrigin(pieceSize, pieceSize);
                    oPiece.setPosition(centerX, centerY);
                    oPiece.setFillColor(Color::Transparent);
                    oPiece.setOutlineThickness(pieceThickness);
                    oPiece.setOutlineColor(Color(50, 50, 200)); // Blue O
                    window.draw(oPiece);
                }
            }
        }

        

        // Draw game status below the board
        const float statusY = offsetY + boardSize + 20.f;

        if (gameOver)
        {
            string statusText;
            if (winner == 1)
            {
                statusText = "Player 1 (X) Wins!";
            }
            else if (winner == 2)
            {
                statusText = vsAI ? "AI (O) Wins!" : "Player 2 (O) Wins!";
            }
            else
            {
                statusText = "It's a Draw!";
            }

            if (tournamentMode)
            {
                statusText += "\nTournament Score: " + to_string(player1Wins) + " - " + to_string(player2Wins);
                if (gamesPlayed == tournamentGames)
                {
                    statusText += "\n" + string(player1Wins > player2Wins ? "Player 1 Wins Tournament!" : (player2Wins > player1Wins ? (vsAI ? "AI Wins Tournament!" : "Player 2 Wins Tournament!") : "Tournament Tied!"));
                }
            }

            drawString(window, statusText, 200, 55, 24, Color::Black);
            drawString(window, "Click to continue...", offsetX, statusY, 20, Color(100, 100, 100));
        }
        else
        {
            string turnText = "Current Turn: ";
            turnText += currentPlayer == 1 ? "Player 1 (X)" : (vsAI ? "AI (O)" : "Player 2 (O)");

            if (timeLimit != Time::Zero)
            {
                Time remaining = timeLimit - gameClock.getElapsedTime();
                int seconds = static_cast<int>(remaining.asSeconds());
                if (seconds < 0)
                    seconds = 0;
                turnText += "\t\tTime Left: " + to_string(seconds) + "s";
            }

            drawString(window, turnText, 200, 55, 24, Color::Black);
        }
    }
    bool isGameOver() const { return gameOver; }
    int getPlayer1Wins() const { return player1Wins; }
    int getPlayer2Wins() const { return player2Wins; }
};

// ----------------- Menu Class -----------------
class Menu
{
private:
    Object background;
    Button Back;
    Button profile;
    Button vsComputer;
    Button vsPlayer;
    Button vsTime;
    Font font;
    TicTacToe ticTacToe;

    int selectTournamentType(RenderWindow &window)
    {
        Object background;
        // Invisible buttons covering areas where grid size options would be
        Button size4x4(365, 195, 310, 65);  // Position for 4x4 selection
        Button size6x6(365, 295, 310, 65);  // Position for 6x6 selection
        Button size8x8(365, 395, 310, 65);  // Position for 8x8 selection
        Button backButton(20, 552, 81, 81); // Back button in top-left corner

        if (!background.load("assets/t_mode.png"))
        {
            cerr << "Failed to load mode selection background" << endl;
            return -1;
        }

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    window.close();
                    return -1;
                }

                // Check clicks on invisible buttons
                if (size4x4.isClicked(event, window))
                {
                    return 3;
                }
                else if (size6x6.isClicked(event, window))
                {
                    return 5;
                }
                else if (size8x8.isClicked(event, window))
                {
                    return 7;
                }
                else if (backButton.isClicked(event, window))
                {
                    return -1; // Back button pressed
                }
            }

            // Rendering - just the background (buttons are invisible)
            window.clear();
            background.draw(window);
            window.display();
        }
        return -1; // Should never reach here
    }

    int modeSelection(RenderWindow &window)
    {
        Object timeSelectBg;
        if (!timeSelectBg.load("assets/mode.png"))
        {
            cerr << "Failed to load time selection background" << endl;
            return -1;
        }

        // Create time selection buttons (centered)
        const float buttonWidth = 308;
        const float buttonHeight = 65;
        const float centerX = window.getSize().x / 2 - buttonWidth / 2;

        Button grid_3x3(365, 193, buttonWidth, buttonHeight, Color::Transparent);
        Button grid_4x4(365, 294, buttonWidth, buttonHeight, Color::Transparent);
        Button grid_5x5(365, 400, buttonWidth, buttonHeight, Color::Transparent);

        bool timeSelected = false;
        while (window.isOpen() && !timeSelected)
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    window.close();
                    return -1;
                }

                if (event.type == Event::MouseButtonPressed &&
                    event.mouseButton.button == Mouse::Left)
                {

                    if (grid_3x3.isClicked(event, window))
                    {
                        cout << "3x3 Mode Selected" << endl;
                        return 3;
                    }
                    else if (grid_4x4.isClicked(event, window))
                    {
                        cout << "4x4 Mode Selected" << endl;
                        return 4;
                    }
                    else if (grid_5x5.isClicked(event, window))
                    {
                        cout << "5x5 Mode Selected" << endl;
                        return 5;
                    }
                }
            }

            window.clear();
            timeSelectBg.draw(window);

            // Draw buttons
            grid_3x3.draw(window);
            grid_4x4.draw(window);
            grid_5x5.draw(window);

            window.display();
        }
        return -1;
    }

    void showTimeSelection(RenderWindow &window, Time &timeLimit)
    {
        Object timeSelectBg;
        if (!timeSelectBg.load("assets/time_select.png"))
        {
            cerr << "Failed to load time selection background" << endl;
            return;
        }

        // Create time selection buttons (centered)
        const float buttonWidth = 308;
        const float buttonHeight = 65;
        const float centerX = window.getSize().x / 2 - buttonWidth / 2;

        Button time1Button(365, 193, buttonWidth, buttonHeight, Color::Transparent);
        Button time2Button(365, 294, buttonWidth, buttonHeight, Color::Transparent);
        Button time3Button(365, 400, buttonWidth, buttonHeight, Color::Transparent);

        bool timeSelected = false;
        while (window.isOpen() && !timeSelected)
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    window.close();
                    return;
                }

                if (event.type == Event::MouseButtonPressed &&
                    event.mouseButton.button == Mouse::Left)
                {

                    if (time1Button.isClicked(event, window))
                    {
                        timeLimit = seconds(60);
                        timeSelected = true;
                    }
                    else if (time2Button.isClicked(event, window))
                    {
                        timeLimit = seconds(120);
                        timeSelected = true;
                    }
                    else if (time3Button.isClicked(event, window))
                    {
                        timeLimit = seconds(180);
                        timeSelected = true;
                    }
                }
            }

            window.clear();
            timeSelectBg.draw(window);

            // Draw buttons
            time1Button.draw(window);
            time2Button.draw(window);
            time3Button.draw(window);

            window.display();
        }
    }

public:
    Menu() : background(),
             Back(18, 543, 80, 80, Color::Transparent), // Invisible but clickable
             profile(355, 160, 310, 65),
             vsComputer(355, 265, 310, 65),
             vsPlayer(355, 365, 310, 65),
             vsTime(355, 465, 310, 65)

    {
        // Load resources
        if (!background.load("assets/menu.png"))
        {
            cerr << "Failed to load menu background image" << endl;
        }

        if (!font.loadFromFile("assets/font.ttf"))
        {
            cerr << "Failed to load font" << endl;
        }
    }

    void classicMode(RenderWindow &window)
    {
        int gridSelection = modeSelection(window);
        if (gridSelection == -1)
            return;

        ticTacToe.startClassicMode(gridSelection);
        runGame(window);
        currentPlayer.updatePlayerScore(currentPlayer.username, ticTacToe.getPlayer1Wins());
    }

    void AIMode(RenderWindow &window)
    {
        int gridSelection = modeSelection(window);
        if (gridSelection == -1)
            return;

        ticTacToe.startAIMode(gridSelection);
        runGame(window);
        currentPlayer.updatePlayerScore(currentPlayer.username, ticTacToe.getPlayer1Wins());
    }

    void tournamentMode(RenderWindow &window)
    {
        int gridSelection = modeSelection(window);
        if (gridSelection == -1)
            return;

        int tournamentType = selectTournamentType(window);
        if (tournamentType == -1)
            return;

        ticTacToe.startTournamentMode(gridSelection, tournamentType);
        runGame(window);
        currentPlayer.updatePlayerScore(currentPlayer.username, ticTacToe.getPlayer1Wins());
    }

    void speedMode(RenderWindow &window)
    {
        int gridSelection = modeSelection(window);
        if (gridSelection == -1)
            return;

        Time timeLimit(seconds(60));
        showTimeSelection(window, timeLimit);

        ticTacToe.startSpeedMode(gridSelection, timeLimit);
        runGame(window);
        currentPlayer.updatePlayerScore(currentPlayer.username, ticTacToe.getPlayer1Wins());
    }

    void runGame(RenderWindow &window)
    {
        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    window.close();
                    return;
                }

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
                {
                    if (ticTacToe.isGameOver())
                    {
                        return; // Return to menu when game is over and player clicks
                    }
                    else
                    {
                        // Calculate board position
                        const float boardSize = 500.f;
                        const float offsetX = (window.getSize().x - boardSize) / 2.f;
                        const float offsetY = (window.getSize().y - boardSize) / 2.f;

                        Vector2i mousePos = Mouse::getPosition(window);

                        // Check if click is within the game board
                        if (mousePos.x >= offsetX && mousePos.x < offsetX + boardSize &&
                            mousePos.y >= offsetY && mousePos.y < offsetY + boardSize)
                        {

                            int gridSize = ticTacToe.getGridSize();
                            float cellSize = boardSize / gridSize;
                            int col = static_cast<int>((mousePos.x - offsetX) / cellSize);
                            int row = static_cast<int>((mousePos.y - offsetY) / cellSize);

                            ticTacToe.handleClick(row, col);
                        }
                    }
                }
            }

            // Update game state (for timed mode)
            ticTacToe.update();

            // Draw everything
            window.clear(Color(230, 230, 230)); // Light gray background

            // Draw the game board
            ticTacToe.draw(window);

            window.display();
        }
    }
    void handleMenu(RenderWindow &window)
    {

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    window.close();
                    exit(EXIT_SUCCESS);
                }

                if (Back.isClicked(event, window))
                {
                    exit(EXIT_SUCCESS);
                }
                else if (profile.isClicked(event, window))
                {
                    cout << "Classic Mode Clicked" << endl;
                    classicMode(window);
                    currentPlayer.updatePlayerScore(currentPlayer.username, player1Score);
                }
                else if (vsComputer.isClicked(event, window))
                {
                    cout << " AI Mode clicked" << endl;
                    AIMode(window);
                    currentPlayer.updatePlayerScore(currentPlayer.username, player1Score);
                }
                else if (vsPlayer.isClicked(event, window))
                {
                    cout << " Tournament clicked" << endl;
                    tournamentMode(window);
                    currentPlayer.updatePlayerScore(currentPlayer.username, player1Score);
                }
                else if (vsTime.isClicked(event, window))
                {
                    cout << " Speed Battle Clicked clicked" << endl;
                    speedMode(window);
                    currentPlayer.updatePlayerScore(currentPlayer.username, player1Score);
                }
            }

            // Rendering
            window.clear();
            background.draw(window);

            window.display();
        }
    }
};

// ----------------- Game Class -----------------
class Game
{
private:
    RenderWindow window;
    Object background;
    Button loginButton;
    Button signInButton;
    Login loginScreen;
    SignIn signInScreen;
    Menu menu;

public:
    Game() : window(VideoMode(1000, 650), "Memory Matching Game"),
             loginButton(377, 260, 255, 66),
             signInButton(377, 365, 255, 66)
    {
        if (!background.load("assets/entering.png"))
        {
            cerr << "Failed to load background image" << endl;
        }
    }

    void run()
    {
        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();

                if (loginButton.isClicked(event, window))
                {
                    string username = loginScreen.usernameTaking(window);
                    if (!username.empty())
                    {
                        currentPlayer = Player::loadFromFile(username);
                        cout << "Logged in as: " << currentPlayer.username << endl;
                        menu.handleMenu(window);
                        return;
                    }
                }
                else if (signInButton.isClicked(event, window))
                {
                    string username = signInScreen.usernameTaking(window);
                    if (!username.empty())
                    {
                        currentPlayer = Player(username);
                        cout << "Signed up as: " << currentPlayer.username << endl;
                        menu.handleMenu(window);
                        return;
                    }
                }
            }

            window.clear();
            background.draw(window);
            window.display();
        }
    }
};

// ----------------- Main -----------------
int main()
{
    Game game;
    game.run();
    return 0;
}