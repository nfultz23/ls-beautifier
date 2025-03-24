#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUF_SIZE 2048

#define VIS_STD "\e[38;5;7m"
#define HDN_STD "\e[38;5;242m"

#define VIS_SPC "\e[38;5;178m"
#define HDN_SPC "\e[38;5;130m"

#define VIS_DIR "\e[38;5;27m"
#define HDN_DIR "\e[38;5;19m"

#define VIS_MFL "\e[38;5;128m"
#define HDN_MFL "\e[38;5;55m"

#define VIS_BKP "\e[38;5;196m"
#define HDN_BKP "\e[38;5;124m"

#define VIS_LNK "\e[38;5;81m"
#define HDN_LNK "\e[38;5;37m"

#define VIS_EXE "\e[38;5;82m"
#define HDN_EXE "\e[38;5;34m"


typedef struct fileentry entry;
struct fileentry {
  entry* prev;
  entry* next;

  char permissions[256];
  char group[256];
  char owner[256];
  char size[256];
  char date[256];
  char filename[256];
  char dest[256];

  int isdir;
  int isln;
  int isexe;
  int hidden;
  int backup;
  int autosave;

  char extension[256];
  int priority;
};


entry* loadentry(char*);
void printentry(entry*, char*);
entry* sortlist(entry*);
int extpriority(char*);


int main(int argc, char** argv) {  
  char line[BUF_SIZE];

  fgets(line, BUF_SIZE, stdin);
  printf("%s", line);

  entry* head;
  if (fgets(line, BUF_SIZE, stdin)) {
    head = loadentry(line);
  } else {
    return 0;
  }

  entry* currentry = head;
  while (fgets(line, BUF_SIZE, stdin)) {
    currentry->next = loadentry(line);
    currentry->next->prev = currentry;
    currentry = currentry->next;
  }

  head = sortlist(head);

  currentry = head;
  while (currentry) {
    printentry(currentry, argv[1]);
    currentry = currentry->next;
  };

  currentry = head;
  while (currentry) {
    entry* delentry = currentry;
    currentry = currentry->next;
    free(delentry);
  }
  
  return 0;
}


void printentry(entry* currentry, char* user) {
  //Print the permissions and group in grey
  printf("%s%s", HDN_STD, currentry->permissions);
  printf("%s%s", HDN_STD, currentry->group);
  //Print the owner in green if the user is the owner, white otherwise
  if (strcmp(currentry->owner, user) == 0)
    printf("%s%s", VIS_EXE, currentry->owner);
  else printf("%s%s", VIS_STD, currentry->owner);
  //Print the size in grey
  printf("%s%s", HDN_STD, currentry->size);
  //Print the date in blue
  printf("%s%s", HDN_DIR, currentry->date);
  
  if (currentry->isdir) {
    //Print directories in blue
    if (currentry->hidden) printf("%s%s", HDN_DIR, currentry->filename);
    else printf("%s%s", VIS_DIR, currentry->filename);

  } else if (currentry->isln) {
    //Print the links in cyan
    if (currentry->hidden) printf("%s%s", HDN_LNK, currentry->filename);
    else printf("%s%s", VIS_LNK, currentry->filename);

  } else if (currentry->isexe) {
    //Print the executable in green
    if (currentry->hidden) printf("%s%s", HDN_EXE, currentry->filename);
    else printf("%s%s", VIS_EXE, currentry->filename);

  } else if (currentry->backup) {
    //Print the backups in red
    if (currentry->hidden) printf("%s%s", HDN_BKP, currentry->filename);
    else printf("%s%s", VIS_BKP, currentry->filename);

  } else if (strcmp(currentry->filename, "makefile") == 0 ||
	     strcmp(currentry->filename, ".makefile") == 0) {
    //Print makefiles in purple
    if (currentry->hidden) printf("%s%s", HDN_MFL, currentry->filename);
    else printf("%s%s", VIS_MFL, currentry->filename);

  } else if (strcmp(currentry->extension, ".yml") == 0) {
    //Print YAML files in the special color
    if (currentry->hidden) printf("%s%s", HDN_SPC, currentry->filename);
    else printf("%s%s", VIS_SPC, currentry->filename);

  } else if (strcmp(currentry->extension, ".sh") == 0) {
    //Print Shell files in the special color
    if (currentry->hidden) printf("%s%s", HDN_SPC, currentry->filename);
    else printf("%s%s", VIS_SPC, currentry->filename);

  } else if (strcmp(currentry->extension, ".gz") == 0 || strcmp(currentry->extension, ".tar") == 0) {
    //Print Tarballs in the special color
    if (currentry->hidden) printf("%s%s", HDN_SPC, currentry->filename);
    else printf("%s%s", VIS_SPC, currentry->filename);

  } else if (strcmp(currentry->extension, ".zip") == 0) {
    //Print zip archives in the special color
    if (currentry->hidden) printf("%s%s", HDN_SPC, currentry->filename);
    else printf("%s%s", VIS_SPC, currentry->filename);

  } else if (strcmp(currentry->filename, "README") == 0 ||
	     strcmp(currentry->filename, "README.txt") == 0 ||
	     strcmp(currentry->filename, "README.md") == 0) {
    //Print README files in the specialc olor
    printf("%s%s", VIS_SPC, currentry->filename);
    
  } else {
    //Print the filename in white if no others
    if (currentry->hidden) printf("%s%s", HDN_STD, currentry->filename);
    else printf("%s%s", VIS_STD, currentry->filename);
    
  }
  //Reset the color to white for the next line
  printf("%s", VIS_STD);

  //Print the link destination in dark blue if applicable
  if (currentry->isln) printf("%s%s%s", HDN_DIR, currentry->dest, VIS_STD);

  printf("\n");
}


entry* loadentry(char* line) {
  entry* newentry = (entry*)malloc( sizeof(entry) );
  memset(newentry, 0, sizeof(entry));

  int x = 0, offset = 0;

  //Loads the permissions section from the line
  while (line[x+offset] != ' ') {
    newentry->permissions[x] = line[x+offset];
    x++;
  };
  offset += x;

  //Loads the group section from the line
  x = 0;
  while (line[x+offset] == ' ') {
    newentry->group[x] = line[x+offset];
    x++;
  }
  while (line[x+offset] != ' ') {
    newentry->group[x] = line[x+offset];
    x++;
  }
  while (line[x+offset] == ' ') {
    newentry->group[x] = line[x+offset];
    x++;
  }
  offset += x;

  //Loads the owner section from the line
  x = 0;
  while (line[x+offset] != ' ') {
    newentry->owner[x] = line[x+offset];
    x++;
  }
  offset += x;

  //Loads the size section from the line
  x = 0;
  while (line[x+offset] == ' ') {
    newentry->size[x] = line[x+offset];
    x++;
  }
  while (line[x+offset] != ' ') {
    newentry->size[x] = line[x+offset];
    x++;
  }
  offset += x;

  //Loads the date section from the line
  x = 0;
  for (int y = 0; y < 3; y++) {
    while (line[x+offset] == ' ') {
      newentry->date[x] = line[x+offset];
      x++;
    }
    while (line[x+offset] != ' ') {
      newentry->date[x] = line[x+offset];
      x++;
    }
  }
  while (line[x+offset] == ' ') {
    newentry->date[x] = line[x+offset];
    x++;
  }
  offset += x;

  //Loads the file name from the line
  x = 0;
  while (line[x+offset] != '\n') {
    if (line[x+offset+1] == '-' && line[x+offset+2] == '>') break;
    newentry->filename[x] = line[x+offset];
    x++;
  }
  offset += x;

  //Sets the bits in the entry struct to signal metadata about the file
  newentry->isdir = (newentry->permissions[0] == 'd');
  newentry->isln  = (newentry->permissions[0] == 'l');
  newentry->isexe = (newentry->permissions[3] == 'x' ||
		     newentry->permissions[6] == 'x' ||
		     newentry->permissions[9] == 'x');
  newentry->hidden = (newentry->filename[0] == '.');
  newentry->backup = (newentry->filename[strlen(newentry->filename)-1] == '~');
  newentry->autosave = (newentry->filename[0] == '#' ||
			newentry->filename[1] == '#');

  //If the item is a link, parse the link's destination
  if (newentry->isln) {
    x = 0;
    while (line[x+offset] != '\n') {
      newentry->dest[x] = line[x+offset];
      x++;
    }
    offset += x;
  }

  //Store the file extension
  int namelen = strlen(newentry->filename) - 1;
  int hasext = 0;
  for (int y = 1; y <= namelen; y++)
    if (newentry->filename[y] == '.') hasext = 1;

  if (hasext) {
    int extlen = 0;
    while (newentry->filename[namelen - extlen] != '.') extlen++;

    for (int y = 0; y <= extlen; y++)
      newentry->extension[y] = newentry->filename[y + (namelen - extlen)];
  }

  //Return a pointer to the entry
  return newentry;
}


entry* sortlist(entry* head) {
  entry* tracer = head;

  while (tracer) {
    tracer->priority = 0;
    if (strcmp(tracer->filename, "README") == 0 ||
	strcmp(tracer->filename, "README.txt") == 0 ||
	strcmp(tracer->filename, "README.md") == 0)
      tracer->priority += 9999999;
    if (tracer->isdir) tracer->priority += 900000;
    if (tracer->isln)  tracer->priority += 800000;
    if (strcmp(tracer->filename, "makefile") == 0 ||
	strcmp(tracer->filename, ".makefile") == 0 ||
	strcmp(tracer->filename, "makefile~") == 0)
      tracer->priority += 700000;
    if (tracer->isexe) tracer->priority += 600000;

    if (tracer->hidden)   tracer->priority -= 12000;
    if (tracer->backup)   tracer->priority -= 25000;
    if (tracer->autosave) tracer->priority -= 50000;

    if (tracer->extension[0] != '\0')
      tracer->priority += extpriority(tracer->extension);

    tracer = tracer->next;
  }

  tracer = head;

  int sorted = 0;
  while (!sorted) {
    //Iterate through the list and sift the priorities upward
    while (tracer->prev) tracer = tracer->prev;

    while (tracer->next) {
      entry* next = tracer->next;

      if (tracer->priority < next->priority) {
	if (tracer->prev) tracer->prev->next = next;
	if (next->next) next->next->prev = tracer;

	next->prev = tracer->prev;
	tracer->next = next->next;

	next->next = tracer;
	tracer->prev = next;

	/* } else if (tracer->priority == next->priority) {
	int len = strlen(tracer->filename);
	if (len > strlen(next->filename)) len = strlen(next->filename);

	int swap = 0;
	for (int y = 0; y < len; y++)
	  if (tracer->filename[y] > next->filename[y]) swap = 1;

	if (swap) {
	  if (tracer->prev) tracer->prev->next = next;
	  if (next->next) next->next->prev = tracer;

	  next->prev = tracer->prev;
	  tracer->next = next->next;

	  next->next = tracer;
	  tracer->prev = next;
	} //*/
	
      }
	
      if (tracer->next) tracer = tracer->next;
    }

    //Check if the list is sorted
    while (tracer->prev) tracer = tracer->prev;

    sorted = 1;
    while (tracer->next) {
      if (tracer->priority < tracer->next->priority) sorted = 0;
      /* else if (tracer->priority == tracer->next->priority) {
	int len = strlen(tracer->filename);
	if (len > strlen(tracer->next->filename)) len = strlen(tracer->next->filename);
	
	for (int y = 0; y < len; y++)
	  if (tracer->filename[y] > tracer->next->filename[y]) sorted = 0;
      } //*/
      if (!sorted) break;
      tracer = tracer->next;
    }
  }

  while (tracer->prev) tracer = tracer->prev;
  return tracer;
}


int extpriority(char* extension) {
  int len = strlen(extension);

  int priority = 0;
  for (int x = 1; x < len; x++)
    priority += (int)extension[x] - (int)('A');

  return -1 * priority * 10;
}
