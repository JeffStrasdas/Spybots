#ifndef MOUSEINTERACTION_H
#define MOUSEINTERACTION_H

namespace Mouse
{
class Button
{
public:
    Button(int ID, int x, int y, int width, int height, Button* previous = NULL):ID(ID), x_min(x), y_min(y)
    {
        x_max = x_min + width;
        y_max = y_min + height;
        next = NULL;
        if(previous != NULL)
            previous->next = this;
    };
    ~Button()
    {
        if(next != NULL)
            delete next;
    };
    Button* Check(int mx,int my)
    {
        if(mx >= x_min && mx <= x_max && my >= y_min && my <= y_max)
            return this;
        if(next != NULL)
            return next->Check(mx,my);
        return NULL;
    }

private:
    int x_min, x_max, y_min, y_max;
    int ID;

    Button* next;

};
}

#endif
