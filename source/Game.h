#pragma once

#include <tinyxml2.h>
#include <SFML/Audio/Sound.hpp>

#include "Item/Elevator/Elevator.h"
#include "Item/Factory.h"
#include "Item/Item.h"
#include "Sky.h"
#include "Sound.h"
#include "Sprite.h"
#include "State.h"
#include "Time.h"
#include "TimeWindow.h"
#include "ToolboxWindow.h"

namespace OT {
	namespace Item { class AbstractPrototype; }
	
	class Game : public State
	{
	public:
		Application & app;
		Game(Application & app);
		virtual ~Game();
		
		typedef std::set<Sprite *> Sprites;
		Sprites sprites;
		
		unsigned int drawnSprites;
		
		void activate();
		void deactivate();
		
		bool handleEvent(sf::Event & event);
		void advance(double dt);
		
		Item::Factory itemFactory;
		
		typedef std::set<Item::Item *> ItemSet;
		ItemSet items;
		Item::Item * mainLobby;
		void addItem(Item::Item * item);
		void removeItem(Item::Item * item);
		
		int funds;
		int rating;
		int population;
		bool populationNeedsUpdate;
		void transferFunds(int f, std::string message = std::string());
		void setFunds(int f);
		void setRating(int r);
		void setPopulation(int p);
		void ratingMayIncrease();
		
		ToolboxWindow toolboxWindow;
		TimeWindow    timeWindow;
		
		Time time;
		int speedMode;
		void setSpeedMode(int sm);
		
		std::string selectedTool;
		int2 toolPosition;
		Item::AbstractPrototype * toolPrototype;
		Item::Item * itemBelowCursor;
		void selectTool(const char * tool);
		
		Sky sky;
		
		Item::Elevator::Elevator * draggingElevator;
		int draggingMotor;
		
		Sound cockSound;
		Sound morningSound;
		Sound bellsSound;
		Sound eveningSound;
		
		typedef std::set<Sound *> SoundSet;
		SoundSet autoreleaseSounds;
		SoundSet playingSounds;
		void playOnce(Path sound);
		
		void updateRoutes();
		Route findRoute(Item::Item * start, Item::Item * destination);
		Route findRoute(Route route, int floor, Item::Item * current, Item::Item * destination);
		
		Route visualizeRoute;
		
	private:
		double zoom;
		double2 poi;
		
		Rocket::Core::ElementDocument * mapWindow;
		
		void reloadGUI();
		
		void encodeXML(tinyxml2::XMLPrinter & xml);
		void decodeXML(tinyxml2::XMLDocument & xml);
	};
}
