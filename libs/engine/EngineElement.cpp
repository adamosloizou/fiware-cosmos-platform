
#include "au/xml.h"         // au::xml...


#include "engine/EngineElement.h"   // Own interface

NAMESPACE_BEGIN(engine)


EngineElement::EngineElement()
{
    // Flag to indicate that this element will be executed just once
    repeated = false;
    
    description = "Engine element to be executed once";
    shortDescription = "Engine element to be executed once";

}

EngineElement::EngineElement( int seconds )
{
    repeated = true;
    period = seconds;
    counter = 0;
    
    std::ostringstream txt;
    txt << "Engine element repeated every " << seconds  << " seconds";
    description = txt.str();
    shortDescription = txt.str();
}

// Reschedule action once executed

void EngineElement::Reschedule()
{
    // Reset cronometer
    cronometer.reset();

    // Increse the counter to get an idea of the number of times a repeated task is executed
    counter++;
}

bool EngineElement::isRepeated()
{
    return repeated;
}

double EngineElement::getTimeToTrigger()
{
    // Time for the next execution
    return period - cronometer.diffTime(); 
}

double EngineElement::getWaitingTime()
{
    // Time for the next execution
    return cronometer.diffTime(); 
}


std::string EngineElement::getDescription()
{
    if( repeated )
    {
        return au::str( "[ Engine element '%s' to be executed in %02.2f seconds ( repeat every %d secs , repeated %d times )] " 
                       , description.c_str()
                       , getTimeToTrigger()
                       , period
                       , counter
                       );
    }
    else
    {
        return au::str( "[ Engine element '%s' ]" , description.c_str() );
    }
    //return description;
}


// get xml information
void EngineElement::getInfo( std::ostringstream& output)
{
    au::xml_open(output, "engine_element");
    au::xml_simple(output, "description", getDescription() );
    au::xml_close(output, "engine_element");
}

NAMESPACE_END