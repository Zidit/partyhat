
#include "animation.h"
#include <stdlib.h>


void animationUpdateRandom(){
	for(uint8_t i = 0; i < 8; i++){
    	vectors[i].direction += 2;
    	vectors[i].r += rand() & 0x03;
    	vectors[i].g += rand() & 0x03;
    	vectors[i].b += rand() & 0x03;
	}
}

void animationInitRandom(){
    vectors[0].direction = 32;
    vectors[0].sector = 4;

    vectors[1].direction = 64;
    vectors[1].sector = 4;

    vectors[2].direction = 96;
    vectors[2].sector = 4;

    vectors[3].direction = 0;
    vectors[3].sector = 4;

    vectors[4].direction = -32;
    vectors[4].sector = 4;

    vectors[5].direction = -64;
    vectors[5].sector = 4;

    vectors[6].direction = -96;
    vectors[6].sector = 4;

    vectors[7].direction = -128;
    vectors[7].sector = 4;

	srand(12);
}


void animationUpdateDefault(){
    vectors[0].direction += 1;
    vectors[1].direction += 2;
    vectors[2].direction += 3;
}

void animationInitDefault(){
    vectors[0].r = 255;
    vectors[0].g = 0;
    vectors[0].b = 0;
    vectors[0].direction = 0;
    vectors[0].sector = 6;

    vectors[1].r = 0;
    vectors[1].g = 255;
    vectors[1].b = 0;
    vectors[1].direction = 83;
    vectors[1].sector = 6;

    vectors[2].r = 0;
    vectors[2].g = 0;
    vectors[2].b = 255;
    vectors[2].direction = -83;
    vectors[2].sector = 6;
}

void animationUpdatePolice(){
    static int8_t dir = 15;
    vectors[0].r += dir;
    vectors[1].b -= dir;
    if (vectors[0].r == 0) dir = 15;
    if (vectors[1].b == 0) dir = -15;
}

void animationInitPolice(){
    vectors[0].r = 0;
    vectors[0].g = 0;
    vectors[0].b = 0;
    vectors[0].direction = 0;
    vectors[0].sector = 7;

    vectors[1].r = 0;
    vectors[1].g = 0;
    vectors[1].b = 255;
    vectors[1].direction = -128;
    vectors[1].sector = 7;

}



uint8_t currentAnimation;


void selectAnimation(const uint8_t id){
    currentAnimation = id;

    for (uint8_t i = 0; i < NUMBER_OF_VECTORS; i++){
        vectors[i].r = 0;
        vectors[i].g = 0;
        vectors[i].b = 0;
        vectors[i].direction = 0;
        vectors[i].sector = 0;
    }


    switch (currentAnimation){
    case 1:
        animationInitDefault();
        break;

    case 2:
        animationInitPolice();
        break;
    case 3:
        animationInitRandom();
        break;

    default:
        break;

    }
}

void updateAnimation(const uint32_t time){

    switch (currentAnimation){
    case 1:
        animationUpdateDefault();
        break;

    case 2:
        animationUpdatePolice();
        break;
    case 3:
        animationUpdateRandom();
        break;


    default:
        break;

    }


}

