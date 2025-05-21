#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	double                duration;
	double                tileid;
} Tiled2cFrame;

typedef struct {
	const char            *name;
	const char            *type;
	const char            *valuestring;
	double                valuenumber;
} Tiled2cProperty;

typedef struct {
	const char            *name;
	const char            *type;
	unsigned int          id;
	unsigned int          tile;
	double                x;
	double                y;
	double                width;
	double                height;
	double                rotation;
	unsigned int          visible;
	unsigned int          numproperties;
	const Tiled2cProperty *properties;
} Tiled2cObject;

typedef struct {
	unsigned int          id;
	const char            *type;
	double                probability;
	int                   numframes;
	const Tiled2cFrame    *frames;
	unsigned int          numproperties;
	const Tiled2cProperty *properties;
	unsigned int          numobjects;
	const Tiled2cObject   *objects;
} Tiled2cTile;

typedef struct {
	const char            *name;
	const char            *path;
	unsigned int          imagewidth;
	unsigned int          imageheight;
	unsigned int          tilewidth;
	unsigned int          tileheight;
	unsigned int          margin;
	unsigned int          columns;
	unsigned int          tilecount;
	unsigned int          numtiles;
	const Tiled2cTile     *tiles;
} Tiled2cSet;

typedef struct {
	const unsigned int    *data;
} Tiled2cTilelayer;

typedef struct {
	const char            *image;
	int                   x;
	int                   y;
} Tiled2cImagelayer;

typedef struct {
	unsigned int          numobjects;
	const Tiled2cObject   *objects;
} Tiled2cObjectLayer;

typedef struct Tiled2cLayer Tiled2cLayer;
typedef struct {
	unsigned int          numlayers;
	const Tiled2cLayer    *layers;
} Tiled2cGroupLayer;

struct Tiled2cLayer {
	const char            *name;
	const char            *class_;
	unsigned int          type;
	double                opacity;
	unsigned int          visible;
	double                parallaxx;
	double                parallaxy;
	unsigned int          numproperties;
	const Tiled2cProperty *properties;
	union {
		Tiled2cTilelayer   tilelayer;
		Tiled2cImagelayer  imagelayer;
		Tiled2cObjectLayer objectlayer;
		Tiled2cGroupLayer  grouplayer;
	};
};

typedef struct {
	const char            *path;
	unsigned int          width;
	unsigned int          height;
	unsigned int          tilewidth;
	unsigned int          tileheight;
	unsigned int          numlayers;
	const Tiled2cLayer    *layers;
	unsigned int          numproperties;
	const Tiled2cProperty *properties;
} Tiled2cMap;

typedef struct {
	const char            *path;
	int                   width;
	int                   height;
	int                   x;
	int                   y;
} Tiled2cWorldMap;

typedef struct {
	const unsigned int    nummaps;
	const Tiled2cWorldMap *maps;
} Tiled2cWorld;

#ifdef __cplusplus
}
#endif
