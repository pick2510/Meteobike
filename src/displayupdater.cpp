#include "displayupdater.h"


displayupdater::displayupdater(threadsafe_queue<results> &inqueue, std::unique_ptr<UBYTE> image):
queue(inqueue), image(std::move(image))
{
    EPD_2IN7_Clear();
    Paint_NewImage(Image.get(), EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 0, WHITE);
    Paint_Clear(WHITE);
    

}

displayupdater::~displayupdater()
{
}
