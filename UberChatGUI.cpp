#include <ncurses.h>
#include <form.h>
#include <string>
#include <iostream>
#include <string.h>
#include <ctime>

using namespace std;

int MAX_MESSAGE_LENGTH = 132; //MAX LENGTH FOR ENTIRE OUTPUT (INCLUDES UUID AND NICK)
int R_MAX = 40;
int C_MAX = 121;
int MAX_UUID_LENGTH = 10;
int CHATROOMS_MAX = 10;
int CHATROOMS_NAME_MAX = 25;
int NICK_SIZE_MAX = 8;

FORM* msgbox;
FORM* chatbox;
FORM* chatroomList;
FORM* usersList;
FORM* userInput;
FIELD* mbf[2];
FIELD* cbf[2];
FIELD* crf[2];
FIELD* ulf[3];
FIELD* inf[1];


const string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%X", &tstruct);

    return buf;
}

void createChatroom(string cr_name){ //DO NOT USE THIS
    char ch;
    int i=0;
    while((ch=getch())!=KEY_BACKSPACE&&ch!=27){
        if(ch==KEY_BACKSPACE){
        cr_name.erase(cr_name.length(),1);
        form_driver(userInput,REQ_DEL_PREV);      
        if(i!=0){
            i--;
        }
        }
        else{
        cr_name.push_back(ch);
        form_driver(userInput, ch);
        i++;
        }
    }
    
    if(ch==10){
    
    }
    form_driver(userInput, REQ_CLR_FIELD);
    form_driver(msgbox, REQ_END_FIELD);
}



void printMsg(char* nick, char* ID, char *input, int size) //CALL THIS TO PRINT A MESSAGE TO THE GUI CHATBOX
{
    int i;
    int count=0;
    static char* lastUserID;
    static char* lastUserNick;

    form_driver(chatbox, REQ_END_FIELD);       //cursor to end of chatbox
    form_driver(chatbox, REQ_NEXT_LINE);       //cursor to next line
    if(lastUserID!=ID || lastUserNick!=nick){
        for(i=0; i<((MAX_MESSAGE_LENGTH/3)-1); i++, count++){
            if(nick[i] == NULL)
                break;
            form_driver(chatbox, nick[i]);
        }form_driver(chatbox, '[');

        for(i=0; count<((MAX_MESSAGE_LENGTH/3)-4); i++, count++){
            if(nick[i] == NULL)
                break;
            form_driver(chatbox, ID[i]);
        }form_driver(chatbox, ']');

        form_driver(chatbox, REQ_NEXT_LINE);form_driver(chatbox, REQ_NEXT_LINE); 
    }
    for(i=0; i<currentDateTime().length(); i++){
        form_driver(chatbox, currentDateTime()[i]);
    }
    form_driver(chatbox, ':'); form_driver(chatbox, ' '); 

    for(i=0; i<size; i++){                 //iterates through to input[count]
        form_driver(chatbox, input[i]);      //sets prints input[i] to Chatbox
    }

    lastUserID = ID;
    lastUserNick = nick;
    refresh();
}

void updateChatRooms(){
    
}

int main()
{

    //--------------------------------------------------------------------------------------
    
    int ch;
    char msg[MAX_MESSAGE_LENGTH];        // **THIS IS THE STRING YOU WANT TO GIVE ALL MESSAGES TO**
    string input;
    int i;
    int count=0;				 //counter for number of characters in the input string(for printing)
    int function=0;
    char nick[] = "xxxxxxxxxxxxx"; 		 //place holder for user nick string
    char ID[] = "012345";                //place holder for user ID string
    char curCR[] = "(Current Chatroom)"; //place holder for current user chatroom

  //SEND LOCAL USER TO CHAT DAEMON

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    resizeterm(R_MAX, C_MAX);
    refresh();

    mbf[0] = new_field(3,(MAX_MESSAGE_LENGTH/3),3,2,0,0);      //creating message box field
    mbf[1] = NULL;
    cbf[0] = new_field(30,(MAX_MESSAGE_LENGTH/3),8,2,0,0);     //creating chatbox field
    cbf[1] = NULL;		
    crf[0] = new_field(10,(CHATROOMS_NAME_MAX+1),3,78,0,0);     //creating chatroom list field 
    crf[1] = NULL;    
    ulf[0] = new_field(20,(8+1),15,53,0,0);                    //creating user list field CR
    ulf[1] = new_field(20,(8+1),15,88,0,0);                    //creating user list field ALL
    ulf[2] = NULL;
    inf[0] = new_field(1,(61),37,59,0,0);                      //creating cmd input field
    inf[1] = NULL;

    field_opts_off(mbf[0], O_AUTOSKIP);        //setting message box options
    field_opts_on(mbf[0], O_WRAP);	              //"

    field_opts_off(cbf[0], O_AUTOSKIP);        //setting chatbox options
    field_opts_off(cbf[0], O_STATIC);            //"
    field_opts_on(cbf[0], O_WRAP);	             //"

    field_opts_off(crf[0], O_AUTOSKIP);	     //setting chatroom list options
    set_field_back(crf[0], A_UNDERLINE);
    
    field_opts_off(ulf[0], O_AUTOSKIP);        //setting user list options
    set_field_back(ulf[0], A_UNDERLINE);
    field_opts_off(ulf[0], O_STATIC);

    field_opts_off(ulf[1], O_AUTOSKIP);
    set_field_back(ulf[1], A_UNDERLINE);
    field_opts_off(ulf[1], O_STATIC);

    field_opts_off(inf[0], O_AUTOSKIP);         //setting input field options
    set_field_back(inf[0], A_UNDERLINE);
    field_opts_off(inf[0], O_STATIC);

    msgbox = new_form(mbf);		//declaring forms
    post_form(msgbox);			    //"
    chatbox = new_form(cbf);		//"
    post_form(chatbox);			    //"
    chatroomList = new_form(crf);   //"
    post_form(chatroomList);        //"
    usersList = new_form(ulf);      //"
    post_form(usersList);           //"
    userInput = new_form(inf);      //"
    post_form(userInput);           //"
    
    refresh();				 

    attron(A_BOLD);                      //drawing horizontal lines
    move(1,0); hline('_', C_MAX-1);       //"
    move(6,0); hline('_', 51);	          //"	
    move(38,0); hline('_', 52);           //"
    
    move(13,52); hline('_', 68);  	      //"
    move(36,52); hline('_', 68);		  //"
    

    move(2, 51); vline('|', 37);	       //drawing vertical lines
    move(0,0); vline('|', 39);		        //"
    move(0, C_MAX-1); vline('|', 39);	    //"
    move(2, 76); vline('|', 12);		    //"
    move(14, 86); vline('|', 23);           //"
    move(38,51); hline('/',70);             //"

    attron(A_STANDOUT);                  //printing titles
    mvprintw(7, 1, "Chatbox:");		        //"
    mvprintw(2, 1, "Enter Message: ");	    //"
    mvprintw(0, 1, "SuperChat:");		    //"
    mvprintw(2, 52, "Controls");		    //"
    mvprintw(2, 77, "Chatrooms");	        //"
    mvprintw(14, 52, "Chatroom Users"); 	    //"
    mvprintw(14, 87, "All Users");	    //"

    attroff(A_STANDOUT);
    mvprintw(37, 52, "Input: ");

    mvprintw(3, 52, "'ESC' to Exit Client");          //Printing Controls
    mvprintw(4, 52, "'Arw(Up/Dn' to Scr Users");       //"
    mvprintw(5, 52, "'Pg(Up/Dn)' to Scr CR");          //"
    mvprintw(6, 52, "'F2' to Create Chatroom");        //"
    mvprintw(7, 52, "'F3' to Change Nick");            //"
    mvprintw(8, 52, "'F4' to Change Chatroom");        //"

    attroff(A_BOLD);
    mvprintw(0, 11, " %s: %s", nick, ID); //print user nick and UUID
    

    //**FROM HERE DOWN IS IMPORTANT FOR ANYONE NOT WORKING ON THE GUI
    //------------------------------------------------------------------------------------------------------------------

    while((ch=getch())!=27)           //Loop for function key input
    {   switch(ch){                   //switch case based on which key was pressed by user
            case KEY_DOWN:             //press down arrow
                if(function==0){
                    form_driver(msgbox, REQ_NEXT_LINE);	 
                }	    
            break;
    
            case KEY_UP:               //press up arrow
                if(function==0)
                    form_driver(msgbox, REQ_PREV_LINE);
                break;

            case KEY_LEFT:             //press left arrow
                if(function==0)
                    form_driver(msgbox, REQ_LEFT_CHAR);
                else
                    form_driver(userInput, REQ_LEFT_CHAR);
                break;
        
            case KEY_RIGHT:            //press right arrow
                if(function==0)
                    form_driver(msgbox, REQ_RIGHT_CHAR);
                else
                    form_driver(userInput, REQ_RIGHT_CHAR);
            break;

            case KEY_BACKSPACE:        //Backspace pressed
                if(function==0){
                    if(count>=MAX_MESSAGE_LENGTH)
                        form_driver(msgbox, REQ_DEL_CHAR);
                    else{
                        form_driver(msgbox, REQ_DEL_PREV);
                    }
                    if(count!=0)
                        count--;
                }
                if(function==2&&count!=0){
                    form_driver(userInput, REQ_DEL_PREV);
                    input.erase(input.length()-1,1);
                    count--;
                }
            break;

            //THIS IS WHERE USER WILL PRESS ENTER AFTER ENTERING INPUT BASED ON THE FUNCTION NUMBER
            //FOR EXAMPLE function=2 MEANS THE INPUT IS FOR F2
            //function = 3 MEANS THE INPUT IS FOR F3
            //etc etc..
            //function = 0 is the standard state of the GUI, this is for sending messages to the chatroom
            case 10: //enter is pressed
                if(function==0){
                    printMsg(nick, ID, msg, count); //print function to print input[] into chatbox
                    count=0;       
                    form_driver(msgbox, REQ_CLR_FIELD);
                    //client->sendMessage(msg);
                    refresh();
                }
                if(function==2){    //if user entered new chatroom name input[]
                    //**CALL CLIENT FUNCTION TO MAKE NEW CHATROOM WITH NAME = input[]**
                    form_driver(chatroomList, REQ_NEXT_LINE);
                    for(i=0; i<input.length(); i++){
                        form_driver(chatroomList, input[i]);
                    }
                    form_driver(userInput, REQ_CLR_FIELD);
                    form_driver(msgbox, REQ_END_FIELD);
                    function=0;
                    count=0;
                }
                if(function==3){    //if user entered new nick input[]
                    //**CALL CLIENT FUNCTION TO CHANGE NICK NAME TO input[]**
                    //**CAUTION: nick IS A char[9] array and input is a type string**
                    for(i=0; i<NICK_SIZE_MAX; i++){
                        if(i<input.length()){
                            nick[i] = input[i];
                        }
                        else{
                            nick[i] = NULL;
                        }
                    }
                    mvprintw(0, 11, " %s: %s", nick, ID);
                    form_driver(userInput, REQ_CLR_FIELD);
                    form_driver(msgbox, REQ_END_FIELD);
                    function=0;
                    count=0;
                }

                if(function==4){    //if user chose a new chatroom to enter
                    //**CALL CLIENT FUNCTION TO PUT USER INTO A NEW CHATROOM OF**
                    form_driver(userInput, REQ_CLR_FIELD);
                    form_driver(msgbox, REQ_END_FIELD);
                    function=0;
                    count=0;
                }
            break;
        
            case KEY_PPAGE:                //Page up pressed
                form_driver(chatbox, REQ_SCR_BLINE);
            break;

            case KEY_NPAGE:                //Page down pressed
                form_driver(chatbox, REQ_SCR_FLINE);
            break;

            case KEY_F(2):                 //F2 pressed
                input.clear();
                function = 2;
                form_driver(msgbox,REQ_CLR_FIELD);
                form_driver(userInput,REQ_END_FIELD);
                count=0;
            break;
            
            case KEY_F(3):                 //F3 pressed
                input.clear();
                function = 3;
                form_driver(msgbox,REQ_CLR_FIELD);
                form_driver(userInput,REQ_END_FIELD);
                count=0;
            break;

            case KEY_F(4):                //F4 pressed
                input.clear();
                function = 4;
                form_driver(msgbox, REQ_CLR_FIELD);
                form_driver(msgbox,REQ_CLR_FIELD);
                form_driver(userInput,REQ_END_FIELD);
                count=0;

            default:
                
                if(function==0){
                    if(count<=MAX_MESSAGE_LENGTH){
                        form_driver(msgbox, ch);
                        msg[count] = ch;
                        if(count<MAX_MESSAGE_LENGTH){
                            count++;
                        }
                        refresh();
                    }
                }
                if(function==2){
                    if(count<25){
                        form_driver(userInput,ch);
                        input.push_back(ch);
                        count++;
                        refresh();
                    }
                }
                if(function==3){
                    if(count<25){
                        form_driver(userInput,ch);
                        input.push_back(ch);
                        count++;
                        refresh();
                    }
                }
                if(function==4){
                    if(count<25){
                        form_driver(userInput,ch);
                        input.push_back(ch);
                        count++;
                        refresh();
                    }
                }
            break;
        }
    }
    unpost_form(msgbox);
    unpost_form(chatbox);
    unpost_form(chatroomList);
    free_form(msgbox);
    free_form(chatbox);
    free_form(chatroomList);
    free_field(mbf[0]);
    free_field(cbf[0]);
    free_field(crf[0]);
    free_field(ulf[0]);
    free_field(ulf[1]);
    //>>SEND LOGOUT SIGNAL TO CLIENT HERE<<

    endwin();
    return 0;
}