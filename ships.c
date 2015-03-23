#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define INIT_SIZE (100)
#define GROWTH_FACTOR (2)
#define MAX_LOAD_FACTOR (1)
#define MULTIPLIER (97)

#include "ships.h"
//#include "hash.h"

struct field
{
	size_t shipCount;
	int size;
	struct elt **table;

};

struct elt
{
	struct elt *next;
	struct ship storedShip;
	struct position filled;
	char shipName;
};

static unsigned long hash_function(unsigned long x, unsigned long y)
{

	unsigned long h;

	h = x * y * MULTIPLIER;

	return h;
}

int positionCompare(struct position p, coord x, coord y) 
{
    if (p.x == x && p.y == y) {
        return 1;
    }
    else {
        return 0;
    }
}

void deleteFromField(struct field *f, struct ship s)
{
	unsigned long x_hash;
	unsigned long y_hash;
	unsigned long h;

	coord x_coord = s.topLeft.x;
	coord y_coord = s.topLeft.y;

	x_hash = (unsigned long) s.topLeft.x;
	y_hash = (unsigned long) s.topLeft.y;

	struct elt **prev;
	struct elt *e;

	if (s.direction == HORIZONTAL) {
		for (int i = 0; i < s.length; i++) {
			x_hash += (unsigned long) i;
			x_coord += (coord) i;
			h = hash_function(x_hash, y_hash) % f->size;

			for (*prev = &(f->table[h]); *prev != 0; prev = &((*prev)->next)) {
				if (!positionCompare((*prev)->filled, x_coord, y_coord)) {
					e = *prev;
					*prev = e->next;

					//free(e->storedShip);
					//free(e->filled);
					free(e);

					break;
				}
			}
		}
	}
	else if (s.direction == VERTICAL) {
		for (int i = 0; i < s.length; i++) {
			y_hash += (unsigned long) i;
			y_coord += (coord) i;
			h = hash_function(x_hash, y_hash) % f->size;

			for (*prev = &(f->table[h]); *prev != 0; prev = &((*prev)->next)) {
				if (positionCompare((*prev)->filled, x_coord, y_coord)) {
					e = *prev;
					*prev = e->next;

					free(e);

					break;
				}
			}
		}
	}
}

struct elt * isContained(struct field *f, coord x, coord y) {
	unsigned long h;
	struct elt *e;

	unsigned long x_hash = (unsigned long) x;
	unsigned long y_hash = (unsigned long) y;

	h = hash_function(x_hash, y_hash) % f->size;

	for (e = f->table[h]; e != 0; e = e->next) {
		if (positionCompare(e->filled, x, y)) {
			return e;
		}
		else {
			return NULL;
		}
	}
	return NULL;
}




struct field * fieldCreateInternal(int size)
{
	struct field *f;

	f = malloc(sizeof(struct field));

	assert(f != 0);

	f->size = size;
	f->shipCount = 0;
	f->table = malloc(sizeof(struct elt *) * (f->size));

	assert(f->table != NULL);

	for (int i = 0; i < f->size; i++) {
		f->table[i] = 0;
	}

	return f;
}

// Dict
// internalDictCreate(int size)
// {
//     Dict d;
//     int i;
//     d = malloc(sizeof(*d)); assert(d != 0);

//     d->size = size;
//     d->n = 0;
//     d->table = malloc(sizeof(struct elt *) * d->size);

//     assert(d->table != 0);
//     for(i = 0; i < d->size; i++) d->table[i] = 0;
    
//     return d; 
// }

void fieldShipGrow(struct field *f, struct ship s) {
	unsigned long x_hash;
	unsigned long y_hash;
	unsigned long h;
	int direction;

	x_hash = (unsigned long) s.topLeft.x;
	y_hash = (unsigned long) s.topLeft.y;

	coord x_coord = s.topLeft.x;
	coord y_coord = s.topLeft.y;

	direction = s.direction;

	struct elt *e;
	e = malloc(sizeof(*e));
	assert(e);

	if (direction == HORIZONTAL) {
		for(int i = 0; i < s.length; i++) {
			x_hash += i;
			x_coord += i;

			if (isContained(f, x_coord, y_coord) != NULL) {
				struct elt *d; 
				d = isContained(f, x_coord, y_coord);
				deleteFromField(f, d->storedShip);
				f->shipCount--;
			}
			e->storedShip = s;
			e->shipName = s.name;
			e->filled.x = x_coord;
			e->filled.y = y_coord;

			h = hash_function(x_coord, y_coord) % f->size;

			e->next = f->table[h];
			f->table[h] = e;

			f->shipCount++;
			}
		}
	if (direction == VERTICAL) {
		for(int i = 0; i < s.length; i++) {
			y_hash += i;
			y_coord += i;

			if (isContained(f, x_coord, y_coord) != NULL) {
				struct elt *d; 
				d = isContained(f, x_coord, y_coord);
				deleteFromField(f, d->storedShip);
				f->shipCount--;
			}
			e->storedShip = s;
			e->shipName = s.name;
			e->filled.x = x_coord;
			e->filled.y = y_coord;

			h = hash_function(x_coord, y_coord) % f->size;

			e->next = f->table[h];
			f->table[h] = e;

			f->shipCount++;
			}
		}
}

static void grow(struct field *f) 
{
    struct field *f2;
    struct field swap;
    struct elt *e;

    f2 = fieldCreateInternal(f->size * GROWTH_FACTOR);

    for (int i = 0; i < f->size; i++) {
        for (e = f->table[i]; e != 0; e = e->next) {
            fieldShipGrow(f, e->storedShip);
        }
    }

    swap = *f;
    *f = *f2;
    *f2 = swap;

    fieldDestroy(f2);
}


void fieldShipInsert(struct field *f, struct ship s) {
	unsigned long x_hash;
	unsigned long y_hash;
	unsigned long h;
	int direction;

	x_hash = (unsigned long) s.topLeft.x;
	y_hash = (unsigned long) s.topLeft.y;

	coord x_coord = s.topLeft.x;
	coord y_coord = s.topLeft.y;

	direction = s.direction;

	struct elt *e;
	e = malloc(sizeof(*e));
	assert(e);

	if (direction == HORIZONTAL) {
		for(int i = 0; i < s.length; i++) {
			x_hash += i;
			x_coord += i;

			if (isContained(f, x_coord, y_coord) != NULL) {
				struct elt *d; 
				d = isContained(f, x_coord, y_coord);
				deleteFromField(f, d->storedShip);
				f->shipCount--;
			}
			e->storedShip = s;
			e->shipName = s.name;
			e->filled.x = x_coord;
			e->filled.y = y_coord;

			h = hash_function(x_coord, y_coord) % f->size;

			e->next = f->table[h];
			f->table[h] = e;

			f->shipCount++;

			if(f->shipCount >= (size_t) f->size * MAX_LOAD_FACTOR) {
				grow(f);
			}
		}
	}
	if (direction == VERTICAL) {
		for(int i = 0; i < s.length; i++) {
			y_hash += i;
			y_coord += i;

			if (isContained(f, x_coord, y_coord) != NULL) {
				struct elt *d; 
				d = isContained(f, x_coord, y_coord);
				deleteFromField(f, d->storedShip);
				f->shipCount--;
			}
			e->storedShip = s;
			e->shipName = s.name;
			e->filled.x = x_coord;
			e->filled.y = y_coord;

			h = hash_function(x_coord, y_coord) % f->size;

			e->next = f->table[h];
			f->table[h] = e;

			f->shipCount++;

			if(f->shipCount >= (size_t) f->size * MAX_LOAD_FACTOR) {
				grow(f);
			}
		}

	}
}



struct field * fieldCreate(void) 
{
	fieldCreateInternal(INIT_SIZE);	
}


void fieldDestroy(struct field *f) 
{
	struct elt *e;
	struct elt *next;

	for (int i = 0; i < f->size; i++) {
		for (e = f->table[i]; e != 0; e = next) {
			next = e->next;

			free(e);
		}
	}

	free(f->table);
	free(f);

}


void fieldPlaceShip(struct field *f, struct ship s)
{

	if (s.length != MAX_SHIP_LENGTH && s.topLeft.x + s.length <= COORD_MAX && s.topLeft.y + s.length <= COORD_MAX) {
		fieldShipInsert(f, s);
	}
}

char fieldAttack(struct field *f, struct position p)
{
	unsigned long h;
	struct elt *e;

	unsigned long x_hash = (unsigned long) p.x;
	unsigned long y_hash = (unsigned long) p.y;
	
	h = hash_function(x_hash, y_hash) % f->size;

	for (e = f->table[h]; e != 0; e = e->next) {
		if (positionCompare(e->filled, p.x, p.y)) {
			deleteFromField(f, e->storedShip);
			f->shipCount--;
			return e->shipName;
		}
		else {
			return NO_SHIP_NAME;
		}
	}
	return NO_SHIP_NAME;
}



size_t fieldCountShips(const struct field *f)
{
	return f->shipCount;
}



