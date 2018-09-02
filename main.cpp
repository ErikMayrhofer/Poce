#include "App.h"
#include "Poce/PoceGame.h"

using namespace std;

int main( int argc, char **argv )
{
    App app;
    app.launch<PoceGame>();
}