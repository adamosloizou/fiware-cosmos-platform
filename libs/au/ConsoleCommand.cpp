


#include "au/ConsoleCommand.h" // Own interface

NAMESPACE_BEGIN(au)

ConsoleCommand::ConsoleCommand()
{
    pos=0;
}

ConsoleCommand::ConsoleCommand( std::string _command )
{
    command = _command;
    pos = command.length();
}

void ConsoleCommand::add( char c )
{
    command.insert(pos, 1, c );
    pos++;
}

void ConsoleCommand::add( std::string txt )
{
    command.insert(pos, txt );
    pos+=txt.length();
}

void ConsoleCommand::delete_char()
{
    //Delete last char introduced ( at pos )
    if( pos > 0 )
    {
        command.erase( pos-1 , 1 );
        pos--;
    }
}

void ConsoleCommand::move_home()
{
    pos = 0;
}

void ConsoleCommand::move_end()
{
    pos = command.length();
}

void ConsoleCommand::move_cursor( int offset )
{
    pos += offset;
    if ( pos < 0 )
        pos = 0;
    if (pos > (int)command.length() )
        pos = command.length();
}

void ConsoleCommand::delete_word()
{
    do {
        delete_char();
    } while ( (pos>0) && ( command[pos-1] != ' ') );
}


void ConsoleCommand::delete_rest_line()
{
    command.erase( pos , command.length() - pos );
}

std::string ConsoleCommand::getCommand()
{
    return command;
}

std::string ConsoleCommand::getCommandUntilPointer()
{
    return command.substr( 0 , pos );
}

void ConsoleCommand::setCommand( std::string _command )
{
    command = _command;
    pos = command.length();
}

int ConsoleCommand::getPos()
{
    return pos;
}

bool ConsoleCommand::isCursorAtEnd()
{
    return (pos == (int)command.length() );
}

NAMESPACE_END