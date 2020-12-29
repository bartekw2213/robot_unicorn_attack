#pragma once
#define UNICORN_TEXTURES_NUM 450						// ilosc tekstur skladajacych sie na animacje biegnacego konia
#define UNICORN_DASHING_TEXTURES_NUM 16					// ilosc tekstur skladajacych sie na anmiacje dashujacego jednorozca
#define UNICORN_TEXTURES_DASH_TO_RUN_SIZE_RATIO 1.5		// ile razy wieksza powinna byc tekstura dashujacego jednorozca od tego biegnacego
#define UNICORN_WIDTH 200								// szerokosc tekstury jednorozca
#define UNICORN_HEIGHT 106								// wysokosc tekstury jednorozca
#define UNICORN_START_SPEED 10							// poczatkowa predkosc jednorozca
#define UNICORN_MAX_SPEED 20							// maksymalna predkosc do jakiej rozpedza sie jednorozec
#define UNICORN_DASH_SPEED 40							// predkosc jaka osiaga jednorozec podczas wykonywania zrywu
#define UNICORN_DASH_TIME 500							// czas trwania zrywu jednorozca
#define UNICORN_TIME_BETWEEN_DASHES 2000				// czas jaki musi minac by gracz ponownie mogl wykonac zryw
#define UNICORN_ACCELERATION_TEMPO 5					// im wi�ksza sta�a tym wolniej gra b�dzie przy�piesza�
#define UNICORN_NORMAL_JUMP_POWER 15					// moc normalnego skoku jednorozca
#define UNICORN_EXTENDED_JUMP_POWER 500					// moc wydluzonego skoku jednorozca
#define UNICORN_MAX_LONG_JUMP_TIME 500					// maksymalny czas trwania dlugiego skoku jednorozca

#define SCREEN_WIDTH 640								// szerokosc ekranu gry
#define SCREEN_HEIGHT 480								// wysokosc ekranu gry
#define GRAVITATION_FACTOR 25							// wspolczynnik grawitacji - im bedzie mniejszy tym grawitacja bedzie silniejsza
#define JUMP_TIME_OFFSET 50								// czas po ktorym jest nasluchiwane czy jednorozec dotknal ziemi po wyskoczeniu

#define PLATFORM_Y SCREEN_HEIGHT - 100					// wysokosc na jakiej ustawiana jest platforma
#define OBSTACLE_COLLISION_Y_OFFSET 10					// granica bledu w jakiej przeszkoda jest czula na kolizje z jednorozcem

#define MAX_PATH_LENGTH 250								// maksymalna dlugosc sciezki pliku
#define MAX_UNICORNE_TEXTURE_FILE_LENGTH 5				// maksymalna dlugosc nazwy pliku z tekstura konia