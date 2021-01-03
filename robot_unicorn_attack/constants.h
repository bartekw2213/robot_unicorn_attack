#pragma once

// === Sta³e dotycz¹ce jednoro¿ca ===
#define UNICORN_TEXTURES_NUM 450						// ilosc tekstur skladajacych sie na animacje biegnacego jednorozca
#define UNICORN_DASHING_TEXTURES_NUM 16					// ilosc tekstur skladajacych sie na anmiacje dashujacego jednorozca
#define UNICORN_FALLING_TEXTURES_NUM 16					// ilosc tekstur skladajacych sie na animacje spadajacego jednorozca
#define UNICORN_EXPLOSION_TEXTURES_NUM 15				// ilosc tekstur skladajacych sie na animacje wybuchajacego jednorozca
#define UNICORN_JUMPING_TEXTURES_NUM 4					// ilosc tekstur skladajacych sie na animacje skaczacego jednorozca

#define UNICORN_WIDTH 200								// szerokosc tekstury jednorozca
#define UNICORN_HEIGHT 106								// wysokosc tekstury jednorozca

#define UNICORN_START_SPEED 10							// poczatkowa predkosc jednorozca
#define UNICORN_MAX_SPEED 20							// maksymalna predkosc do jakiej rozpedza sie jednorozec
#define UNICORN_DASH_SPEED 40							// predkosc jaka osiaga jednorozec podczas wykonywania zrywu
#define UNICORN_ACCELERATION_TEMPO 5					// im wiêksza sta³a tym wolniej gra bêdzie przyœpieszaæ

#define UNICORN_EXPLOSION_TIME 500						// czas trwania eksplozji jednorozca
#define UNICORN_FELLING_OVER_TIME 500					// czas trwania spadania jednorozca poza plansze gry
#define UNICORN_MAX_LONG_JUMP_TIME 500					// maksymalny czas trwania dlugiego skoku jednorozca
#define UNICORN_DASH_TIME 500							// czas trwania zrywu jednorozca
#define UNICORN_MIN_TIME_BETWEEN_DASHES 2000			// czas jaki musi minac by gracz ponownie mogl wykonac zryw

#define UNICORN_NORMAL_JUMP_POWER 15					// moc normalnego skoku jednorozca
#define UNICORN_EXTENDED_JUMP_POWER 700					// moc wydluzonego skoku jednorozca (moc z jaka jednorozec zaczyna wybicie)
#define UNICORN_TEXTURES_DASH_TO_RUN_SIZE_RATIO 1.5		// ile razy wieksza powinna byc tekstura dashujacego jednorozca od tego biegnacego

// === Sta³e dotycz¹ce rozgrywki ===
#define SCREEN_WIDTH 940								// szerokosc ekranu gry
#define SCREEN_HEIGHT 580								// wysokosc ekranu gry

#define UNICORN_WISHES_NUM 3							// ilosc "zyc"
#define GRAVITATION_FACTOR 25							// wspolczynnik grawitacji - im bedzie mniejszy tym grawitacja bedzie silniejsza
#define JUMP_TIME_OFFSET 50								// czas po ktorym jest nasluchiwane czy jednorozec dotknal ziemi po wyskoczeniu
#define NUM_OF_SIGNS_IN_DIGIT_TEXTURE 11				// ilosc znakow znajdujacych sie w teksturze cyfr
#define TIME_DIGITS	6									// ile cyfr bedzie wyswietlac timer

// === Sta³e dotycz¹ce œcie¿ek plików ===
#define MAX_PATH_LENGTH 250								// maksymalna dlugosc sciezki pliku
#define MAX_TEXTURE_FILE_LENGTH 5						// maksymalna dlugosc nazwy pliku z tekstur¹


// === Sta³e dotycz¹ce napisów ===
#define SUBTITLES_TEXTURES_NUM 5						// ilosc tekstur z napisami
#define START_SUBTITLE_INDEX 0							// index tekstury z napisem zapraszajacym do rozpoczecia gry
#define AGAIN_SUBTITLE_INDEX 1							// index tekstury z napisem zapraszajacym do ponownej rozgrywki

// === Sta³e dotycz¹ce platform ===
#define PLATFORM_TYPES 4								// ilosc typow platfrom jakie moga wystapic w rozgrywce
#define DISTANCE_BETWEEN_PLATFORMS 500					// dystans pomiedzy platformami
#define PLATFORMS_Y_WHEN_FELL_OVER_IS_DETECTED -1000	// gdy jednorozec jest w odleglosci 1000 na osi Y od najblizszej platformy - gra jest konczona
#define PLATFORMS_Y_COLLISION_OFFSET 20					// granica na osi Y na jakiej ci¹gle wykrywana jest ewentualna kolizja jednoro¿ca z obiektem
#define MAX_COLLIDERS 3									// maksymalna ilosc prostokatow wykrywajacych kolizje

// Platform 1
#define PLATFORM1_WIDTH 900								// szerokosc platformy typu 1
#define PLATFORM1_HEIGHT 270							// wysokosc platformy typu 1
#define MIN_PLATFORM1_Y SCREEN_HEIGHT - 150				// minimalna wysokosc na jakiej moze pojawic sie platforma typu 1
#define MAX_PLATFORM1_Y SCREEN_HEIGHT + 50				// maksymalna wysokosc na jakiej moze pojawic sie platforma typu 1
#define PLATFORM1_COLLIDERS_NUM 3						// ilosc prostokatow wykrywajacych kolizje z platforma typu 1

// Platform 2
#define PLATFORM2_WIDTH 1000							// szerokosc platformy typu 2
#define PLATFORM2_HEIGHT 287							// wysokosc platformy typu 2
#define MIN_PLATFORM2_Y SCREEN_HEIGHT - 350				// minimalna wysokosc na jakiej moze pojawic sie platforma typu 2
#define MAX_PLATFORM2_Y SCREEN_HEIGHT - 250				// maksymalna wysokosc na jakiej moze pojawic sie platforma typu 2
#define PLATFORM2_COLLIDERS_NUM 2						// ilosc prostokatow wykrywajacych kolizje z platforma typu 2

// Platform 3
#define PLATFORM3_WIDTH 900								// szerokosc platformy typu 3
#define PLATFORM3_HEIGHT 140							// wysokosc platformy typu 3
#define MIN_PLATFORM3_Y SCREEN_HEIGHT - 650				// minimalna wysokosc na jakiej moze pojawic sie platforma typu 3
#define MAX_PLATFORM3_Y SCREEN_HEIGHT - 550				// maksymalna wysokosc na jakiej moze pojawic sie platforma typu 3
#define PLATFORM3_COLLIDERS_NUM 1						// ilosc prostokatow wykrywajacych kolizje z platforma typu 3

// Platform 4
#define PLATFORM4_WIDTH 1000							// szerokosc platformy typu 4
#define PLATFORM4_HEIGHT 460							// wysokosc platformy typu 4
#define MIN_PLATFORM4_Y SCREEN_HEIGHT + 100;			// minimalna wysokosc na jakiej moze pojawic sie platforma typu 4
#define MAX_PLATFORM4_Y SCREEN_HEIGHT + 300				// maksymalna wysokosc na jakiej moze pojawic sie platforma typu 4
#define PLATFORM4_COLLIDERS_NUM 2						// ilosc prostokatow wykrywajacych kolizje z platforma typu 4