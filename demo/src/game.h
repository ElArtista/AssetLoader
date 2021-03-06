/*********************************************************************************************************************/
/*                                                  /===-_---~~~~~~~~~------____                                     */
/*                                                 |===-~___                _,-'                                     */
/*                  -==\\                         `//~\\   ~~~~`---.___.-~~                                          */
/*              ______-==|                         | |  \\           _-~`                                            */
/*        __--~~~  ,-/-==\\                        | |   `\        ,'                                                */
/*     _-~       /'    |  \\                      / /      \      /                                                  */
/*   .'        /       |   \\                   /' /        \   /'                                                   */
/*  /  ____  /         |    \`\.__/-~~ ~ \ _ _/'  /          \/'                                                     */
/* /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                                                      */
/*                   \_|      /        _)   ;  ),   __--~~                                                           */
/*                     '~~--_/      _-~/-  / \   '-~ \                                                               */
/*                    {\__--_/}    / \\_>- )<__\      \                                                              */
/*                    /'   (_/  _-~  | |__>--<__|      |                                                             */
/*                   |0  0 _/) )-~     | |__>--<__|     |                                                            */
/*                   / /~ ,_/       / /__>---<__/      |                                                             */
/*                  o o _//        /-~_>---<__-~      /                                                              */
/*                  (^(~          /~_>---<__-      _-~                                                               */
/*                 ,/|           /__>--<__/     _-~                                                                  */
/*              ,//('(          |__>--<__|     /                  .----_                                             */
/*             ( ( '))          |__>--<__|    |                 /' _---_~\                                           */
/*          `-)) )) (           |__>--<__|    |               /'  /     ~\`\                                         */
/*         ,/,'//( (             \__>--<__\    \            /'  //        ||                                         */
/*       ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'                                          */
/*     `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/                                                     */
/*   ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~                                                       */
/*    ;'( ')/ ,)(                              ~~~~~~~~~~                                                            */
/*   ' ') '( (/                                                                                                      */
/*     '   '  `                                                                                                      */
/*********************************************************************************************************************/
#ifndef _GAME_H_
#define _GAME_H_

#include <vector.h>
#include <linalgb.h>
#include "text_render.h"
#include "camera.h"

struct mesh_handle
{
    unsigned int vao;
    unsigned int vbo;
    unsigned int uvs;
    unsigned int wbo;
    unsigned int ebo;
    unsigned int indice_count;
    unsigned int mat_idx;
};


struct model_handle
{
    struct mesh_handle* meshes;
    unsigned int num_meshes;
    struct skeleton* skel;
    struct frameset* fset;
    struct mesh_group** mesh_groups;
    unsigned int num_mesh_groups;
};

struct game_object
{
    struct model_handle model;
    mat4 transform;
    struct vector diff_textures;
    size_t mat_refs[16];
};

struct game_context
{
    /* Window assiciated with the game */
    struct window* wnd;
    /* Master run flag, indicates when the game should exit */
    int* should_terminate;
    /* GPU data */
    unsigned int prog;
    struct vector gobjects;
    /* Camera state */
    struct camera cam;
    /* Game state data */
    float rotation;
    float rotation_prev;
    int is_rotating;
    size_t cur_obj;
    /* Wireframe visualization */
    int show_wireframe;
    /* Normals visualization */
    int visualizing_normals;
    unsigned int vis_nrm_prog;
    /* Skeleton visualization */
    int visualizing_skeleton;
    unsigned int vis_skel_prog;
    /* Animation */
    float anim_tmr;
    /* Text rendering */
    text_renderer_t* text_rndr;
};

/* Initializes the game instance */
void game_init(struct game_context* ctx);
/* Update callback used by the main loop */
void game_update(void* userdata, float dt);
/* Render callback used by the main loop */
void game_render(void* userdata, float interpolation);
/* De-initializes the game instance */
void game_shutdown(struct game_context* ctx);

#endif // ! _GAME_H_
