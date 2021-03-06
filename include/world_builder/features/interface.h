/*
  Copyright (C) 2018 by the authors of the World Builder code.

  This file is part of the World Builder.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _world_builder_features_interface_h
#define _world_builder_features_interface_h

#include <map>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include <world_builder/world.h>
#include <world_builder/parameters.h>
#include <world_builder/point.h>


using boost::property_tree::ptree;
using namespace std;

namespace WorldBuilder
{
  class World;


  namespace Features
  {
    class ObjectFactory;

    /**
     * This class is an interface for the specific plate tectonic feature classes,
     * such as continental plate, oceanic plate and subduction zone.
     */
    class Interface
    {
      public:
        /**
         * constructor
         */
        Interface();

        /**
         * Destructor
         */
        virtual
        ~Interface();

        /**
         * Declare the entries stored in the interface, which
         * are the name and coordinates.
         */
        void declare_interface_entries(Parameters &prm, const CoordinateSystem coordinate_system);

        /**
         * declare and read in the world builder file into the parameters class
         */
        virtual
        void decare_entries() = 0;


        /**
         * takes temperature and position and returns a temperature.
         */
        virtual
        double temperature(const Point<3> &position,
                           const double depth,
                           const double gravity,
                           double temperature) const = 0;
        /**
         * Returns a value for the requested composition (0 is not present,
         * 1 is present) based on the given position and
         */
        virtual
        double composition(const Point<3> &position,
                           const double depth,
                           const unsigned int composition_number,
                           double value) const = 0;


        /**
         * A function to register a new type. This is part of the automatic
         * registration of the object factory.
         */
        static void registerType(const std::string &name,
                                 ObjectFactory *factory);

        /**
         * A function to create a new type. This is part of the automatic
         * registration of the object factory.
         */
        static std::unique_ptr<Interface> create(const std::string &name, WorldBuilder::World *world);

      protected:
        /**
         * A pointer to the world class to retrieve variables.
         */
        WorldBuilder::World *world;

        /**
         * The name of the feature type.
         */
        std::string name;

        /**
         * number of original coordinates, before adding
         * more automatically.
         */
        unsigned int original_number_of_coordinates;

        /**
         * The coordinates at the surface of the feature
         */
        std::vector<Point<2> > coordinates;

        /**
         * A vector of one dimensional coordinates for this feature.
         * If empty, this variables is interpretated just as
         * {0,1,2,...,number of coordinates}. It allows for, for example,
         * adding extra coordinates automatically, and still reference the
         * user provided coordinates by the original number. Note that no
         * whole numbers may be skiped. So for a list of 4 points, {0,0.5,1,2}
         * is allowed, but {0,2,3,4} is not.
         */
        std::vector<double> one_dimensional_coordinates;


        /**
         * The name of the temperature submodule used by this feature.
         */
        std::string temperature_submodule_name;

        /**
         * The name of the composition submodule used by this feature.
         */
        std::string composition_submodule_name;


      private:
        static std::map<std::string, ObjectFactory *> &get_factory_map()
        {
          static std::map<std::string, ObjectFactory *> factories;
          return factories;
        }
    };


    /**
     * A class to create new objects
     */
    class ObjectFactory
    {
      public:
        virtual std::unique_ptr<Interface> create(World *world) = 0;
    };

    /**
     * A macro which should be in every derived cpp file to automatically
     * register it. Because this is a library, we need some extra measures
     * to ensure that the static variable is actually initialized.
     */
#define WB_REGISTER_FEATURE(klass,name) \
  int make_sure_compilation_unit_referenced##klass() { return 0; } \
  class klass##Factory : public ObjectFactory { \
    public: \
      klass##Factory() \
      { \
        Interface::registerType(#name, this); \
      } \
      virtual std::unique_ptr<Interface> create(World *world) { \
        return std::unique_ptr<Interface>(new klass(world)); \
      } \
  }; \
  static klass##Factory global_##klass##Factory;

    /**
     * A macro which should be in every derived header file to automatically
     * register it. Because this is a library, we need some extra measures
     * to ensure that the static variable is actually initialized.
     */
#define WB_REGISTER_FEATURE_HEADER(klass) \
  extern int make_sure_compilation_unit_referenced##klass(); \
  static int never_actually_used##klass = make_sure_compilation_unit_referenced##klass();


  }
}

#endif
