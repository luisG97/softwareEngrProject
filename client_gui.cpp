// This example will demonstrate the integration of
// the boost asio chatserver with a simple FLTK program
//
//
//
#include <assert.h>

#include "client_cli.cpp"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Display.H>
#include <string>

Fl_Window win   (600, 500, "SimpleChat");
Fl_Input input1 (210, 450, 200, 20, "In: ");
Fl_Button quit  (410, 450, 50,20,"Quit");
Fl_Button clear (470, 450, 50,20,"Clear");

Fl_Text_Buffer *buff = new Fl_Text_Buffer ();
Fl_Text_Display *disp = new Fl_Text_Display (190,50,400,400,"chat");

Fl_Text_Buffer *buff1 = new Fl_Text_Buffer ();
Fl_Text_Display *disp1 = new Fl_Text_Display (2,50,180,250,"Chatroom members");

Fl_Return_Button *b_nick = new Fl_Return_Button(10, 330, 170, 25, "NICK");
Fl_Return_Button *b_change_room = new Fl_Return_Button(10, 365, 170, 25, "CHANGECHATROOM");
Fl_Return_Button *b_req_room = new Fl_Return_Button(10, 400, 170, 25, "REQCHATROOM");
Fl_Return_Button *b_name_room = new Fl_Return_Button(10, 435, 170, 25, "NAMECHATROOM");

void clearmembers ();
static void cb_clear ();
void printrequest (std::string payload);
int function = 0;
/*Fl_Text_Buffer *buff2 = new Fl_Text_Buffer ();
Fl_Text_Display *disp2 = new Fl_Text_Display (2,10,100,35);*/

// boost asio instances
chat_client *c = NULL;
std::thread *t = NULL;
std::vector< std::vector<std::string> >uuidnames; // hold uuid and names

  void storename(std::string name, std::string id)
  {
    std::cout << name <<"*09**"<< std::endl;
    std::cout << id <<"*87**"<< std::endl;
    std::vector<std::string>uuid;
    uuid.push_back(id);
    uuid.push_back(name);
    uuidnames.push_back(uuid);

  } //uuidnames[i].size()

  std::string getname(std::string id)
  {
    std::string ans;
    bool found= true;
    for (int i=0; i < uuidnames.size() &&  found; i++)
    {
      for (int j=0; j < uuidnames.size()&&  found; j++)
      {
        //std::cout<< "outter------------->>>>>>>>>>>>"<<uuidnames.size()<<std::endl;
        //std::cout<< "outter-------------counter>>>>>>>>>>>>"<<j<<std::endl;
        //std::cout<< "array------------->>>>>>>>>>>>"<<uuidnames[i][0]<<std::endl;
        //std::cout<< "outter id------------->>>>>>>>>>>>"<<id<<std::endl;
        if(uuidnames[i][0]==id)
        {
          ans = uuidnames[i][1];
          //std::cout<< "INNER------------->>>>>>>>>>>>"<< ans<<std::endl;
          found = false;
          //std::cout<< "INNER-----if-------->>>>out>>"<< ans<<std::endl;
        }
        //std::cout<< "outer--------if----->>>>out>>"<< ans<<std::endl;
      }
      //std::cout<< "outter------------->>>>out>>"<< ans<<std::endl;
    }
    //std::cout<< "outter-----result-------->>>>out>>"<< ans<<std::endl;
    return ans;
  }

  void setnameid(std::string info)
  {
    int comma = info.find(',');
    while (comma != -1)
    {
      std::string uui = info.substr(0,comma);
      info.erase(0,comma+1);

      comma =0;
      comma=info.find('_');
      std::string username = info.substr(0,comma);
      storename(username, uui);
      username =username + "\n";

      info.erase(0,comma+2);
      if (buff1)
      {
        buff1->append ( username.c_str () );
      }
      if (disp1)
      {
        disp1->show ();
      }
      win.show ();
      comma =0;
      comma=info.find(',');
    }
  }
    void removector()
    {
        int Siz = uuidnames.size();
         uuidnames.erase(uuidnames.begin(),uuidnames.begin()+Siz );

    }


static void cb_recv ( chat_message cm )
{
  // Note, this is an async callback from the perspective
  // of Fltk..
  //
  // high chance of a lock needed here if certain fltk calls
  // are made.  (like show() .... )

  std::string S;

  boost::crc_32_type checksum;
  boost::posix_time::ptime time;
  std::string payload;
  std::string command;

  bool cksum_ok = parse_command ( cm, &checksum, &time, &command, &payload );

   if (command == "REQUUID"||command == "NICK"||command == "NAMECHATROOM")
    {
      chat_message  msg = format_reply ( "GETNICK");
      payload = payload + " \n";

      if (buff)
      {
        buff->append ( payload.c_str () );
      }
      if (disp)
      {
        disp->show ();
        disp->Fl_Text_Display::move_down();
      }
        win.show ();
      }
      else if (command == "REQUSERS"){
        setnameid(payload );

      }

    else if (command == "REQTEXT")
    {
      std::string content = payload + "\n";
      int ANS = content.find('-');
      while (ANS != -1)
      {
        ANS = content.find(' ');
        std::string uuid = content.substr(0,ANS);
        //std::cout<< "REqtext------------->>>>>>>>>>>>"<< uuid<<std::endl;
        std::string nick= getname(uuid);
        // = getname(uuid);
        //std::cout<< "REqtext------------->>>>>>>>>>>>"<< nick<<std::endl;
        content.erase(0,ANS+1);
        ANS = 0;
        ANS = content.find(';');
        std::string messageout=content.substr(0,ANS);
        //std::cout<< "REqtext------------->>>>>>>>>>>>"<< messageout<<std::endl;
        content.erase(0,ANS+1);
        messageout = "["+nick+"]: " + messageout + " \n";


        if (buff)
        {
            buff->append ( messageout.c_str () );
        }
        if (disp)
        {
            disp->show ();
            disp->Fl_Text_Display::move_down();
        }

        win.show ();
        ANS = 0;
        ANS = content.find('-');
      }
    }


  else if (command == "CHANGECHATROOM")
  {
    clearmembers ();
    removector();
    cb_clear ();

    payload = payload + " \n";
    printrequest(payload);

  }

 else if (command == "REQCHATROOMS")
  {
     payload = payload + " \n";
    int a = payload.find(';');
    printrequest(payload);

      //  a = payload.find(' ');

  }


}

 void printrequest (std::string payload) {       //prints to chat main window!!!!
   if (buff)
      {
        buff->append ( payload.c_str () );
      }
      if (disp)
      {
        disp->show ();
        disp->Fl_Text_Display::move_down();
      }
        win.show ();
     }

  void clearmembers ()
  {
    if (buff1)
    {
      buff1->remove (0,  buff1->length () );
      removector();
    }
    // may need to call show() ?

  }

  static void cb_clear ()
  {
    if (buff)
    {
      buff->remove (0,  buff->length () );
    }
    // may need to call show() ?

  }

static void cb_quit ( )
{
  // this is where we exit to the operating system
  // any clean up needs to happen here
  //
  if (c)
  {
    c->close();
  }
  if (t)
  {
     t->join();
  }
  exit (0);
}
static void cb_input1 (Fl_Input*, void * userdata)
{
  chat_message msg;
  msg.body_length(std::strlen( ( const char *) input1.value ()) + 1);
  // ensure it is null terminated
  // std::memset (msg.body(), 0, msg.body_length());
  // copy over the payload
  //std::memcpy (msg.body(), ( const char *) input1.value (), msg.body_length()-1);

  std::string temp = input1.value();

  switch(function){
    case 0:
      msg = format_reply ( "SENDTEXT, " + temp);
    break;

    case 1:
      msg = format_reply ( "NICK, " + temp);
    break;

    case 2:
      msg = format_reply ( "CHANGECHATROOM, " + temp);
    break;

    case 4:
      msg = format_reply ( "NAMECHATROOM, " + temp);
    break;
  }

  //msg = format_reply (temp);
  msg.encode_header();
  std::cout << "sent " << msg.body() << std::endl;
  c->write(msg);
  //std::memset ( input1.value() , 0, sizeof ( input1.value() ));
  function = 0;
  sleep (1);
}

void update(void*){
  chat_message rqtxt;
  rqtxt = format_reply("REQTEXT");
  rqtxt.encode_header();
  c->write(rqtxt);


  Fl::repeat_timeout(0.5, update);
}

void updateUsers(void*){
  clearmembers();
  chat_message rqusers;
  rqusers = format_reply("REQUSERS");
  rqusers.encode_header();
  c->write(rqusers);

  Fl::repeat_timeout(5, updateUsers);
}

void rqUUID(void*){

  chat_message command;
  command = format_reply("REQUUID");
  command.encode_header();
  c->write(command);
}

void function_1(Fl_Widget* w, void* p){
  function = 1;
}

void function_2(Fl_Widget* w, void* p){
  function = 2;
}

void function_3(Fl_Widget* w, void* p){
    chat_message msg;
  msg = format_reply ( "REQCHATROOMS");
  msg.encode_header();
  c->write(msg);
  sleep(1);

  function = 0;
}

void function_4(Fl_Widget* w, void* p){
  function = 4;
}

void function_5(Fl_Widget* w, void* p){
  function = 5;
}

int main ( int argc, char **argv)
{
  win.begin ();
  win.add (input1);

  Fl::add_timeout(0.5, rqUUID);
  Fl::add_timeout(1, updateUsers);
  Fl::add_timeout(2, update);


  input1.callback ((Fl_Callback*)cb_input1,( void *) "Enter next:");
  input1.when ( FL_WHEN_ENTER_KEY );
  b_nick -> callback((Fl_Callback *) function_1, 0);
  b_change_room -> callback((Fl_Callback *) function_2, 0);
  b_req_room -> callback((Fl_Callback *) function_3, 0);
  b_name_room -> callback((Fl_Callback *) function_4, 0);
  quit.callback (( Fl_Callback*) cb_quit );
  clear.callback (( Fl_Callback*) cb_clear );
  win.add (quit);
  disp->buffer(buff);
  disp1->buffer(buff1);

    // disp2->buffer(buff2);
  win.end ();
  win.show ();

  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);

    auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });
    c = new chat_client (io_service, endpoint_iterator, &cb_recv);

    t = new std::thread ([&io_service](){ io_service.run(); });

    // goes here, never to return.....
    return Fl::run ();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  // never gets here
  return 0;
}
