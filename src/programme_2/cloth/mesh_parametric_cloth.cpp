/*
**    TP CPE Lyon
**    Copyright (C) 2015 Damien Rohmer
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**   This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mesh_parametric_cloth.hpp"

#include "../lib/common/error_handling.hpp"
#include <cmath>

namespace cpe
{


void mesh_parametric_cloth::update_force(float Kw, int K)
{

    int const Nu = size_u();
    int const Nv = size_v();
    int const N_total = Nu*Nv;
    ASSERT_CPE(static_cast<int>(force_data.size()) == Nu*Nv , "Error of size");


    //Gravity
    static vec3 const g (0.0f,0.0f,-9.81f);
    vec3 const g_normalized = g/N_total;
    for (int ku = 0; ku < Nu; ++ku)
    {
        for (int kv = 0; kv < Nv; ++kv)
        {   
            force(ku,kv) = g_normalized;
        }
    }
    
    //*************************************************************//
    // TO DO, Calculer les forces s'appliquant sur chaque sommet
    //*************************************************************//

    ////////////////// Q8 //////////////////
    // // Fix the force to 0 for two vertices at the edge of the mesh
    // force(0, 0) = vec3(0.0f, 0.0f, 0.0f); // Top-left corner
    // force(Nu - 1, Nv - 1) = vec3(0.0f, 0.0f, 0.0f); // Bottom-right corner

    ////////////// structural springs //////////////
    //float const K = 20.0f;

    // Choix de L10_rest comme distance entre points adjacents
    float const L10_rest = 1.0f / (Nu - 1);
    float const L12_rest = sqrt(L10_rest * L10_rest + L10_rest * L10_rest);

    for (int ku = 0; ku < Nu; ++ku)
    {
        for (int kv = 0; kv < Nv; ++kv)
        {   
            // Ajout des forces de ressorts structuraux aux 4 voisins directs
            // dans tout les cas on défini 4 voisins et on vérifie si ils sont dans les limites du maillage
            // si ils sont dans les limites on calcule la force de ressort et on l'ajoute à la force totale
            // si ils ne sont pas dans les limites on ne fait rien

            // fonction pour calculer et appliquer la force de ressort entre deux points
            auto apply_spring_force = [&](int ku_neigh, int kv_neigh, float L10_rest) {
                if (ku_neigh < 0 || ku_neigh >= Nu || kv_neigh < 0 || kv_neigh >= Nv) // Si le voisin n'est pas dans les limites du maillage, on ne fait rien
                    return;
                vec3 const u0 = vertex(ku, kv) - vertex(ku_neigh, kv_neigh);
                float const L10 = norm(u0);
                vec3 const f10 = K * (L10_rest -L10) * u0 / L10;
                force(ku, kv) += f10;
            };

            // Voisin direct 
            apply_spring_force(ku + 1, kv, L10_rest);
            apply_spring_force(ku, kv + 1, L10_rest);
            apply_spring_force(ku - 1, kv, L10_rest);
            apply_spring_force(ku, kv - 1, L10_rest);

            // Voisin diagonal
            apply_spring_force(ku + 1, kv + 1,L12_rest);
            apply_spring_force(ku - 1, kv + 1,L12_rest);
            apply_spring_force(ku + 1, kv - 1,L12_rest);
            apply_spring_force(ku - 1, kv - 1,L12_rest);

            // Voisin espacé de 2 sommet
            apply_spring_force(ku + 2, kv, 2*L10_rest);
            apply_spring_force(ku, kv + 2, 2*L10_rest);
            apply_spring_force(ku - 2, kv, 2*L10_rest);
            apply_spring_force(ku, kv - 2, 2*L10_rest);

            // Intersections avec le plan
            if (vertex(ku, kv).z() <= -1.101f)
            {
                vertex(ku, kv).z() = -1.10f; // On projette le sommet sur le plan (un peu au dessus pour éviter les intersections)
                speed(ku, kv).z() = 0.0f; // On annule la composante orthogonale de la vitesse
                force(ku, kv).z() = 0.0f; // On annule la composante orthogonale de la force
            }
            
            // Intersactions avec la sphère
            float radius = 0.2f;                    // Rayon de la sphère (un peu plus grand que la sphère pour éviter les intersections)
            vec3 center = {0.5f, 0.05f, -1.1f};     // Centre de la sphère
            vec3 u = vertex(ku, kv) - center;       // Vecteur entre le sommet et le centre de la sphère
            float d = norm(u);                      // Distance entre le sommet et le centre de la sphère
            if (d <= radius)
            {
                vec3 n = u / d;                       // Vecteur normalisé entre le sommet et le centre de la sphère
                vertex(ku, kv) = center + radius * n; // On projette le sommet sur la sphère
                vec3 v = speed(ku, kv);
                vec3 v_t = v - dot(v, n) * n;         // Composante tangentielle de la vitesse
                speed(ku, kv) = v_t;                  // On annule la composante orthogonale de la vitesse
            }

            // Effet du vent
            //float Kw = 0.02f; // Constante intensité du vent
            vec3 uw = {0.0f, -1.0f, 0.0f}; // Vecteur de direction du vent
            vec3 n = normal(ku, kv); // Vecteur normal au plan
            force(ku, kv) += Kw * dot(n, uw) * n; 
        
        }
    }

    // Fix the force to 0 for two vertices at the edge of the mesh
    force(0, 0) = vec3(0.0f, 0.0f, 0.0f); // Top-left corner
    force(Nu - 1, 0) = vec3(0.0f, 0.0f, 0.0f); // Bottom-right corner


}

void mesh_parametric_cloth::integration_step(float const dt, float const mu)
{
    ASSERT_CPE(speed_data.size() == force_data.size(),"Incorrect size");
    ASSERT_CPE(static_cast<int>(speed_data.size()) == size_vertex(),"Incorrect size");


    int const Nu = size_u();
    int const Nv = size_v();
    //*************************************************************//
    // TO DO: Calculer l'integration numerique des positions (euler excplicite) au cours de l'intervalle de temps dt.
    //*************************************************************//

    // static float const mu = 0.2f;   // Coefficient d'amortissement

    for (int ku = 0; ku < Nu; ++ku)
    {
        for (int kv = 0; kv < Nv; ++kv)
        {
            // Update speed
            vec3& v = speed(ku, kv);
            vec3 const& f = force(ku, kv);
            v = (1-mu*dt)*v + dt * f;

            // Update position
            vec3& p = vertex(ku, kv);
            p = p + dt * v;
        }
    }


    //security check (throw exception if divergence is detected)
    static float const LIMIT=30.0f;
    for(int ku=0 ; ku<Nu ; ++ku)
    {
        for(int kv=0 ; kv<Nv ; ++kv)
        {
            vec3 const& p = vertex(ku,kv);

            if( norm(p) > LIMIT )
            {
                throw exception_divergence("Divergence of the system",EXCEPTION_PARAMETERS_CPE);
            }
        }
    }

}

void mesh_parametric_cloth::set_plane_xy_unit(int const size_u_param,int const size_v_param)
{
    mesh_parametric::set_plane_xy_unit(size_u_param,size_v_param);

    int const N = size_u()*size_v();
    speed_data.resize(N);
    force_data.resize(N);
}

vec3 const& mesh_parametric_cloth::speed(int const ku,int const kv) const
{
    ASSERT_CPE(ku >= 0 , "Value ku ("+std::to_string(ku)+") should be >=0 ");
    ASSERT_CPE(ku < size_u() , "Value ku ("+std::to_string(ku)+") should be < size_u ("+std::to_string(size_u())+")");
    ASSERT_CPE(kv >= 0 , "Value kv ("+std::to_string(kv)+") should be >=0 ");
    ASSERT_CPE(kv < size_v() , "Value kv ("+std::to_string(kv)+") should be < size_v ("+std::to_string(size_v())+")");

    int const offset = ku + size_u()*kv;

    ASSERT_CPE(offset < static_cast<int>(speed_data.size()),"Internal error");

    return speed_data[offset];
}

vec3& mesh_parametric_cloth::speed(int const ku,int const kv)
{
    ASSERT_CPE(ku >= 0 , "Value ku ("+std::to_string(ku)+") should be >=0 ");
    ASSERT_CPE(ku < size_u() , "Value ku ("+std::to_string(ku)+") should be < size_u ("+std::to_string(size_u())+")");
    ASSERT_CPE(kv >= 0 , "Value kv ("+std::to_string(kv)+") should be >=0 ");
    ASSERT_CPE(kv < size_v() , "Value kv ("+std::to_string(kv)+") should be < size_v ("+std::to_string(size_v())+")");

    int const offset = ku + size_u()*kv;

    ASSERT_CPE(offset < static_cast<int>(speed_data.size()),"Internal error");

    return speed_data[offset];
}

vec3 const& mesh_parametric_cloth::force(int const ku,int const kv) const
{
    ASSERT_CPE(ku >= 0 , "Value ku ("+std::to_string(ku)+") should be >=0 ");
    ASSERT_CPE(ku < size_u() , "Value ku ("+std::to_string(ku)+") should be < size_u ("+std::to_string(size_u())+")");
    ASSERT_CPE(kv >= 0 , "Value kv ("+std::to_string(kv)+") should be >=0 ");
    ASSERT_CPE(kv < size_v() , "Value kv ("+std::to_string(kv)+") should be < size_v ("+std::to_string(size_v())+")");

    int const offset = ku + size_u()*kv;

    ASSERT_CPE(offset < static_cast<int>(force_data.size()),"Internal error");

    return force_data[offset];
}

vec3& mesh_parametric_cloth::force(int const ku,int const kv)
{
    ASSERT_CPE(ku >= 0 , "Value ku ("+std::to_string(ku)+") should be >=0 ");
    ASSERT_CPE(ku < size_u() , "Value ku ("+std::to_string(ku)+") should be < size_u ("+std::to_string(size_u())+")");
    ASSERT_CPE(kv >= 0 , "Value kv ("+std::to_string(kv)+") should be >=0 ");
    ASSERT_CPE(kv < size_v() , "Value kv ("+std::to_string(kv)+") should be < size_v ("+std::to_string(size_v())+")");

    int const offset = ku + size_u()*kv;

    ASSERT_CPE(offset < static_cast<int>(force_data.size()),"Internal error");

    return force_data[offset];
}

void mesh_parametric_cloth::reset_cloth()
{
    int const Nu = size_u();
    int const Nv = size_v();
    for (int ku = 0; ku < Nu; ++ku)
    {
        for (int kv = 0; kv < Nv; ++kv)
        {
            vertex(ku, kv) = {ku * 1.0f / (Nu - 1), kv * 1.0f / (Nv - 1), 0.0f};
            speed(ku, kv) = {0.0f, 0.0f, 0.0f};
            force(ku, kv) = {0.0f, 0.0f, 0.0f};
        }
    }
}


}
