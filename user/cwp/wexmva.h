/* This file is automatically generated. DO NOT EDIT! */

#ifndef _wexmva_h
#define _wexmva_h


#include "wex.h"
#include "wextap.h"
#include "wexeic.h"
#include "wexmva.h"
#include "wexssr.h"
#include "wexlsr.h"
#include "wexslo.h"
#include "wexutl.h"


/*------------------------------------------------------------*/
wexmvaop3d wexmva_init(wexcub3d cub,
                       wexcip3d cip 
                            );
/*< initialize >*/


/*------------------------------------------------------------*/
void wexmva_close(wexmvaop3d wexmvaop);
/*< free allocated storage >*/


/*------------------------------------------------------------*/
void wexmva(wexmvaop3d      wexmvaop,
            int  adj,         /* forward/adjoint flag         */
            wexcub3d cub,     /* wavefield hypercube          */
            wexssr3d ssr,     /* SSR operator                 */
            wexlsr3d lsr,     /* SSR operator                 */
            wextap3d tap,     /* tapering operator            */
            wexslo3d slo,     /* background slowness          */
            sf_file wfls,     /* source wavefield             */
            sf_file wflr,     /* receiver wavefield           */
            sf_file pswf,   /* perturbed source wavefield   */
            sf_file prwf,   /* perturbed reciever wavefield */
            sf_file pslo      /* slowness perturbation        */
          );
/*< Forward/Adjoint WEMVA operator >*/


/*------------------------------------------------------------*/
void wexzomva(wexmvaop3d      wexmvaop,
            int  adj,         /* forward/adjoint flag         */
            wexcub3d cub,     /* wavefield hypercube          */
            wexssr3d ssr,     /* SSR operator                 */
            wexlsr3d lsr,     /* SSR operator                 */
            wextap3d tap,     /* tapering operator            */
            wexslo3d slo,     /* background slowness          */
            sf_file wflr,     /* receiver wavefield           */
            sf_file prwf,   /* perturbed reciever wavefield */
            sf_file pslo      /* slowness perturbation        */
          );
/*< Forward/Adjoint WEMVA operator >*/

#endif