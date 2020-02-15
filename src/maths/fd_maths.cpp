#include "fd_maths.hpp"

bool FD_Maths::pointInRect(const int x, const int y, const SDL_Rect* r) {
	SDL_Point p{ x, y };
	return FD_Maths::pointInRect(&p, r);
}

bool FD_Maths::pointInRect(const int x, const int y, const FD_Rect* r) {
	FD_Point p{ x, y };
	return FD_Maths::pointInRect(&p, r);
}

bool FD_Maths::pointInRect(const SDL_Point* p, const SDL_Rect* r) {
	if (p == nullptr || r == nullptr) return false;
	if (r->w == 0 || r->h == 0) return false;
	return ((p->x >= r->x) && (p->x <= r->x + r->w)) 
		&& ((p->y >= r->y) && (p->y <= r->y + r->h));
}
bool FD_Maths::pointInRect(const FD_Point* p, const FD_Rect* r) {
	if (p == nullptr || r == nullptr) return false;
	if (r->w == 0 || r->h == 0) return false;
	return ((p->x >= r->x) && (p->x <= r->x + r->w))
		&& ((p->y >= r->y) && (p->y <= r->y + r->h));
}

bool FD_Maths::hasIntersection(const SDL_Rect* a, const SDL_Rect* b) {
	if (a == nullptr || b == nullptr) return false;
	if (a->w == 0 || a->h == 0 || b->w == 0 || b->h == 0) return false;
	if (a->x >= b->x + b->w) return false;
	if (a->x + a->w <= b->x) return false;
	if (a->y >= b->y + b->h) return false;
	if (a->y + a->h <= b->y) return false;
	return true;
}
bool FD_Maths::hasIntersection(const FD_Rect* a, const FD_Rect* b) {
	if (a == nullptr || b == nullptr) return false;
	if (a->w == 0 || a->h == 0 || b->w == 0 || b->h == 0) return false;
	if (a->x >= b->x + b->w) return false;
	if (a->x + a->w <= b->x) return false;
	if (a->y >= b->y + b->h) return false;
	if (a->y + a->h <= b->y) return false;
	return true;
}

bool FD_Maths::intersection(const SDL_Rect* a, const SDL_Rect* b, SDL_Rect* intersection) {
	if (!hasIntersection(a, b)) return false;
	SDL_Point p;
	intersection->x = (a->x > b->x) ? a->x : b->x;
	intersection->y = (a->y > b->y) ? a->y : b->y;
	p.x = (a->x + a->w < b->x + b->w) 
		? a->x + a->w : b->x + b->w;
	p.y = (a->y + a->h < b->y + b->h) 
		? a->y + a->h : b->y + b->h;
	intersection->w = p.x - intersection->x;
	intersection->h = p.y - intersection->y;
	if (intersection->w <= 0) return false;
	if (intersection->h <= 0) return false;
	return true;
}
bool FD_Maths::intersection(const FD_Rect* a, const FD_Rect* b, FD_Rect* intersection) {
	if (!hasIntersection(a, b)) return false;
	SDL_Point p;
	intersection->x = (a->x > b->x) ? a->x : b->x;
	intersection->y = (a->y > b->y) ? a->y : b->y;
	p.x = (a->x + a->w < b->x + b->w)
		? a->x + a->w : b->x + b->w;
	p.y = (a->y + a->h < b->y + b->h)
		? a->y + a->h : b->y + b->h;
	intersection->w = p.x - intersection->x;
	intersection->h = p.y - intersection->y;
	if (intersection->w <= 0) return false;
	if (intersection->h <= 0) return false;
	return true;
}
