#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/geometry.hpp>
#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>

using namespace cv;
using namespace std;

const int W = 1000;
const int H = 600;

const int PADDLE_W = 14;
const int PADDLE_H = 100;
const int BALL_R = 10;

struct Player {
    int y = H / 2;
    int score = 0;
};

struct Ball {
    float x = W / 2.0f;
    float y = H / 2.0f;
    float vx = 5.0f;
    float vy = 3.0f;
};

Point detectColor(Mat& frame, const Scalar& low,
                  const Scalar& high, const string& name) {
    Mat hsv, mask;
    cvtColor(frame, hsv, COLOR_BGR2HSV);

    inRange(hsv, low, high, mask);

    // Supprime une partie du bruit de l'image.
    Mat kernel = getStructuringElement(
        MORPH_ELLIPSE, Size(5, 5));

    morphologyEx(mask, mask, MORPH_OPEN, kernel);
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);

    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL,
                 CHAIN_APPROX_SIMPLE);

    double maxArea = 300.0;
    int best = -1;

    for (int i = 0; i < (int)contours.size(); i++) {
        double area = cv::contourArea(contours[i]);

        if (area > maxArea) {
            maxArea = area;
            best = i;
        }
    }

    if (best == -1)
        return Point(-1, -1);

    cv::Moments m = cv::moments(contours[best]);

    if (m.m00 == 0)
        return Point(-1, -1);

    Point center(
        (int)(m.m10 / m.m00),
        (int)(m.m01 / m.m00)
    );

    circle(frame, center, 12,
           name == "VERT" ? Scalar(0, 255, 0)
                          : Scalar(255, 0, 0), 3);

    putText(frame, name, Point(center.x + 12, center.y),
            FONT_HERSHEY_SIMPLEX, 0.6,
            Scalar(255, 255, 255), 2);

    return center;
}

void resetBall(Ball& ball, int direction) {
    ball.x = W / 2.0f;
    ball.y = H / 2.0f;
    ball.vx = 5.0f * direction;
    ball.vy = (rand() % 7 - 3);

    if (abs(ball.vy) < 2)
        ball.vy = 2.0f;
}

enum class GameState {
    MENU,
    COUNTDOWN,
    PLAYING,
    GAME_OVER
};

void drawCenteredText(Mat& image, const string& text, int y,
                     double scale, const Scalar& color,
                     int thickness = 2) {
    int baseline = 0;
    Size size = getTextSize(text, FONT_HERSHEY_SIMPLEX,
                            scale, thickness, &baseline);
    putText(image, text,
            Point((image.cols - size.width) / 2, y),
            FONT_HERSHEY_SIMPLEX, scale, color, thickness,
            LINE_AA);
}

void startCountdown(Player& left, Player& right, Ball& ball,
                    GameState& state,
                    chrono::steady_clock::time_point& stateStart) {
    left.score = 0;
    right.score = 0;
    resetBall(ball, rand() % 2 == 0 ? -1 : 1);
    state = GameState::COUNTDOWN;
    stateStart = chrono::steady_clock::now();
}

int main() {
    VideoCapture camera(0);
    if (!camera.isOpened()) {
        cerr << "Erreur : impossible d'ouvrir la webcam." << endl;
        return 1;
    }

    camera.set(CAP_PROP_FRAME_WIDTH, 640);
    camera.set(CAP_PROP_FRAME_HEIGHT, 480);

    Player left, right;
    Ball ball;
    GameState state = GameState::MENU;
    int targetScore = 5;
    int winner = 0;
    auto stateStart = chrono::steady_clock::now();

    // Couleurs HSV : vert et bleu.
    Scalar greenLow(35, 70, 70);
    Scalar greenHigh(90, 255, 255);
    Scalar blueLow(95, 80, 50);
    Scalar blueHigh(130, 255, 255);

    namedWindow("PONG", WINDOW_AUTOSIZE);
    namedWindow("WEBCAM", WINDOW_AUTOSIZE);

    const int leftX = 35;
    const int rightX = W - 35 - PADDLE_W;
    bool running = true;

    while (running) {
        Mat frame;
        if (!camera.read(frame) || frame.empty()) {
            cerr << "Erreur de lecture webcam." << endl;
            break;
        }

        // La détection tourne dans tous les états pour garder les
        // raquettes réactives au démarrage et après une pause au menu.
        flip(frame, frame, 1);
        Point green = detectColor(frame, greenLow, greenHigh, "VERT");
        Point blue = detectColor(frame, blueLow, blueHigh, "BLEU");
        if (green.y >= 0)
            left.y = green.y * H / frame.rows;
        if (blue.y >= 0)
            right.y = blue.y * H / frame.rows;

        left.y = clamp(left.y, PADDLE_H / 2, H - PADDLE_H / 2);
        right.y = clamp(right.y, PADDLE_H / 2, H - PADDLE_H / 2);

        const auto now = chrono::steady_clock::now();
        if (state == GameState::COUNTDOWN) {
            const double elapsed = chrono::duration<double>(
                now - stateStart).count();
            if (elapsed >= 4.0)
                state = GameState::PLAYING;
        }

        if (state == GameState::PLAYING) {
            // Physique existante de la balle et des raquettes.
            ball.x += ball.vx;
            ball.y += ball.vy;

            if (ball.y - BALL_R <= 0 || ball.y + BALL_R >= H)
                ball.vy = -ball.vy;

            if (ball.vx < 0 &&
                ball.x - BALL_R <= leftX + PADDLE_W &&
                ball.x + BALL_R >= leftX &&
                ball.y >= left.y - PADDLE_H / 2 &&
                ball.y <= left.y + PADDLE_H / 2) {
                ball.x = leftX + PADDLE_W + BALL_R;
                ball.vx = abs(ball.vx) * 1.05f;
                ball.vy += (ball.y - left.y) * 0.08f;
                ball.vy = clamp(ball.vy, -9.0f, 9.0f);
            }

            if (ball.vx > 0 &&
                ball.x + BALL_R >= rightX &&
                ball.x - BALL_R <= rightX + PADDLE_W &&
                ball.y >= right.y - PADDLE_H / 2 &&
                ball.y <= right.y + PADDLE_H / 2) {
                ball.x = rightX - BALL_R;
                ball.vx = -abs(ball.vx) * 1.05f;
                ball.vy += (ball.y - right.y) * 0.08f;
                ball.vy = clamp(ball.vy, -9.0f, 9.0f);
            }

            if (ball.x < 0) {
                right.score++;
                resetBall(ball, 1);
                if (right.score >= targetScore) {
                    winner = 2;
                    state = GameState::GAME_OVER;
                }
            } else if (ball.x > W) {
                left.score++;
                resetBall(ball, -1);
                if (left.score >= targetScore) {
                    winner = 1;
                    state = GameState::GAME_OVER;
                }
            }
        }

        Mat display(H, W, CV_8UC3, Scalar(20, 20, 20));
        if (state == GameState::MENU) {
            drawCenteredText(display, "PONG VISION", 150, 2.0,
                             Scalar(255, 255, 255), 4);
            drawCenteredText(display, "LED VERTE : joueur gauche",
                             240, 0.8, Scalar(0, 255, 0));
            drawCenteredText(display, "LED BLEUE : joueur droit",
                             285, 0.8, Scalar(255, 150, 40));
            drawCenteredText(display,
                "Score pour gagner : " + to_string(targetScore),
                360, 0.9, Scalar(255, 255, 255));
            drawCenteredText(display,
                "Fleches gauche / droite : choisir le score",
                405, 0.65, Scalar(190, 190, 190));
            drawCenteredText(display, "ESPACE - Demarrer la partie",
                             485, 0.85, Scalar(0, 220, 255));
            drawCenteredText(display, "Echap - Quitter", 535, 0.6,
                             Scalar(170, 170, 170));
        } else if (state == GameState::COUNTDOWN) {
            // Le compte à rebours n'exécute pas la physique et ne marque pas.
            for (int y = 0; y < H; y += 30)
                rectangle(display, Point(W / 2 - 2, y),
                          Point(W / 2 + 2, y + 15),
                          Scalar(100, 100, 100), FILLED);
            rectangle(display, Point(leftX, left.y - PADDLE_H / 2),
                      Point(leftX + PADDLE_W, left.y + PADDLE_H / 2),
                      Scalar(0, 255, 0), FILLED);
            rectangle(display, Point(rightX, right.y - PADDLE_H / 2),
                      Point(rightX + PADDLE_W, right.y + PADDLE_H / 2),
                      Scalar(255, 100, 0), FILLED);
            circle(display, Point((int)ball.x, (int)ball.y), BALL_R,
                   Scalar(255, 255, 255), FILLED);

            const double elapsed = chrono::duration<double>(
                now - stateStart).count();
            const string countdown = elapsed < 3.0
                ? to_string(3 - (int)elapsed) : "GO !";
            drawCenteredText(display, countdown, H / 2 + 45, 3.0,
                             Scalar(0, 220, 255), 5);
        } else {
            for (int y = 0; y < H; y += 30)
                rectangle(display, Point(W / 2 - 2, y),
                          Point(W / 2 + 2, y + 15),
                          Scalar(100, 100, 100), FILLED);
            rectangle(display, Point(leftX, left.y - PADDLE_H / 2),
                      Point(leftX + PADDLE_W, left.y + PADDLE_H / 2),
                      Scalar(0, 255, 0), FILLED);
            rectangle(display, Point(rightX, right.y - PADDLE_H / 2),
                      Point(rightX + PADDLE_W, right.y + PADDLE_H / 2),
                      Scalar(255, 100, 0), FILLED);
            circle(display, Point((int)ball.x, (int)ball.y), BALL_R,
                   Scalar(255, 255, 255), FILLED);

            putText(display, to_string(left.score),
                    Point(W / 2 - 100, 60), FONT_HERSHEY_SIMPLEX,
                    1.5, Scalar(0, 255, 0), 3);
            putText(display, to_string(right.score),
                    Point(W / 2 + 55, 60), FONT_HERSHEY_SIMPLEX,
                    1.5, Scalar(255, 100, 0), 3);

            if (state == GameState::GAME_OVER) {
                const string winnerName = winner == 1
                    ? "Joueur gauche (LED verte) gagne !"
                    : "Joueur droit (LED bleue) gagne !";
                drawCenteredText(display, "PARTIE TERMINEE",
                                 220, 1.1, Scalar(255, 255, 255), 3);
                drawCenteredText(display, winnerName, 290, 0.9,
                    winner == 1 ? Scalar(0, 255, 0)
                                : Scalar(255, 150, 40), 2);
                drawCenteredText(display,
                    "Score final : " + to_string(left.score) +
                        " - " + to_string(right.score),
                    355, 0.85, Scalar(255, 255, 255));
                drawCenteredText(display,
                    "ESPACE - Rejouer    M - Menu    Echap - Quitter",
                    455, 0.65, Scalar(0, 220, 255));
            }
        }

        imshow("PONG", display);
        imshow("WEBCAM", frame);

        const int key = waitKeyEx(16);
        if (key == 27 || key == 'q' || key == 'Q') {
            running = false;
        } else if (state == GameState::MENU) {
            if (key == 2424832 || key == 81)
                targetScore = max(1, targetScore - 1);
            else if (key == 2555904 || key == 83)
                targetScore = min(20, targetScore + 1);
            else if (key == ' ')
                startCountdown(left, right, ball, state, stateStart);
        } else if (state == GameState::GAME_OVER) {
            if (key == ' ')
                startCountdown(left, right, ball, state, stateStart);
            else if (key == 'm' || key == 'M') {
                left.score = 0;
                right.score = 0;
                state = GameState::MENU;
            }
        }
    }

    camera.release();
    destroyAllWindows();
    return 0;
}

