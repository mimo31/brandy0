/**
 * start-state.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#include "start-state.hpp"

namespace brandy0
{

StartState::StartState(ApplicationAbstr *const app)
	: app(app),
	mainWin(this),
	aboutWin(this)
{
}

void StartState::activate()
{
	mainWin.show();
	app->addWindow(mainWin);
}

void StartState::deactivate()
{
	aboutWin.hide();
	mainWin.hide();
}

void StartState::run()
{
	mainWin.show();
	app->run(mainWin);
}

void StartState::goToNewSimulation()
{
	app->enterNewConfig();
}

void StartState::showAbout()
{
	aboutWin.show();
	aboutWin.present();
}

}
