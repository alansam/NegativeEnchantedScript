//
//  ascii-table.c
//  CF.ASCIITable
//
//  Created by Alan Sampson on 4/16/20.
//  Copyright © 2020 Alan @ FreeShell. All rights reserved.
//

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

//  MARK: - Definitions.
enum ascii_ctrl {
  NUL, SOH, STX, ETX, EOT, ENQ, ACK, BEL,
  BS,  TAB, LF,  VT,  FF,  CR,  SO,  SI,
  DLE, DC1, DC2, DC3, DC4, NAK, SYN, ETB,
  CAN, EM,  SUB, ESC, FS,  GS,  RS,  US,
  NL  = LF,
  NP  = FF,
  sp  = ' ',
  DEL = 0x7f,
};

struct control {
  enum ascii_ctrl ctrl;
  char const      name[4];
  char const      desc[30];
};
typedef struct control control;

ssize_t binary_search(control ctrl[], size_t lft, size_t rgt, enum ascii_ctrl seek);

//  MARK: - Constants.
static
control const controls[] = {
  { .ctrl = NUL, .name = "NUL", .desc = "null",                     },
  { .ctrl = SOH, .name = "SOH", .desc = "start of heading",         },
  { .ctrl = STX, .name = "STX", .desc = "start of text",            },
  { .ctrl = ETX, .name = "ETX", .desc = "end of text",              },
  { .ctrl = EOT, .name = "EOT", .desc = "end of transmission",      },
  { .ctrl = ENQ, .name = "ENQ", .desc = "enquiry",                  },
  { .ctrl = ACK, .name = "ACK", .desc = "acknowledge",              },
  { .ctrl = BEL, .name = "BEL", .desc = "bell",                     },
  { .ctrl = BS,  .name = "BS",  .desc = "backspace",                },
  { .ctrl = TAB, .name = "TAB", .desc = "horizontal tab",           },
  { .ctrl = LF,  .name = "LF",  .desc = "line feed (NL, new line)", },
  { .ctrl = VT,  .name = "VT",  .desc = "vertical tab",             },
  { .ctrl = FF,  .name = "FF",  .desc = "form feed (NP, new page)", },
  { .ctrl = CR,  .name = "CR",  .desc = "carriage return",          },
  { .ctrl = SO,  .name = "SO",  .desc = "shift out",                },
  { .ctrl = SI,  .name = "SI",  .desc = "shift in",                 },
  { .ctrl = DLE, .name = "DLE", .desc = "data link escape",         },
  { .ctrl = DC1, .name = "DC1", .desc = "device control 1",         },
  { .ctrl = DC2, .name = "DC2", .desc = "device control 2",         },
  { .ctrl = DC3, .name = "DC3", .desc = "device control 3",         },
  { .ctrl = DC4, .name = "DC4", .desc = "device control 4",         },
  { .ctrl = NAK, .name = "NAK", .desc = "negative acknowledge",     },
  { .ctrl = SYN, .name = "SYN", .desc = "synchronous idle",         },
  { .ctrl = ETB, .name = "ETB", .desc = "end of trans. block",      },
  { .ctrl = CAN, .name = "CAN", .desc = "cancel",                   },
  { .ctrl = EM,  .name = "EM",  .desc = "end of medium",            },
  { .ctrl = SUB, .name = "SUB", .desc = "substitute",               },
  { .ctrl = ESC, .name = "ESC", .desc = "escape",                   },
  { .ctrl = FS,  .name = "FS",  .desc = "file separator",           },
  { .ctrl = GS,  .name = "GS",  .desc = "group separator",          },
  { .ctrl = RS,  .name = "RS",  .desc = "record separator",         },
  { .ctrl = US,  .name = "US",  .desc = "unit separator",           },
  { .ctrl = sp,  .name = " ",   .desc = "Space",                    },
  { .ctrl = DEL, .name = "DEL", .desc = "delete",                   },
};
static
size_t const controls_l = sizeof(controls) / sizeof(*controls);

//  MARK: - Implementation.
/*
 *  MARK: main()
 */
int main(int argc, const char * argv[]) {
  printf("CF.ASCIITable\n");

  printf("  | %-3s %-3s %-3s %-3s %-24s |\n",
         "Dec", "Oct", "Hex", "Chr", "Description");
  printf("  + %-3s %-3s %-3s %-3s %-24s +\n",
         "---", "---", "---", "---", "------------------------");
  fflush(stdout);

  for (int c_ = NUL; c_ <= DEL; ++c_) {
    if (iscntrl(c_)) {
      //  Handle control characters
      ssize_t fnd = binary_search((control *) controls, 0, controls_l, c_);
      if (fnd >= 0) {
        control ths = controls[fnd];
        printf("  | %1$3u %1$03o  %1$02X %2$-3s %3$-24s |\n",
               (unsigned) ths.ctrl, ths.name, ths.desc);
        fflush(stdout);
      }
      else {
        puts("GRONK!");
      }
    }
    else if (isspace(c_)) {
      //  Handle any other space characters
      ssize_t fnd = binary_search((control *) controls, 0, controls_l, c_);
      if (fnd >= 0) {
        control ths = controls[fnd];
        printf("  | %1$3u %1$03o  %1$02X %2$-3s %3$-24s |\n",
               (unsigned) ths.ctrl, ths.name, ths.desc);
        fflush(stdout);
      }
      else {
        puts("GRONK!");
      }
    }
    else {
      //  The residual are printable characters; handle those
      printf("  | %1$3u %1$03o  %1$02X %1$-3c %2$-24s |\n",
             (unsigned) c_, "");
        fflush(stdout);
    };
  }

  printf("  + %-3s %-3s %-3s %-3s %-24s +\n",
         "---", "---", "---", "---", "------------------------");
  printf("  | %-3s %-3s %-3s %-3s %-24s |\n",
         "Dec", "Oct", "Hex", "Chr", "Description");
  putchar('\n');
  fflush(stdout);

  return 0;
}

/*
 *  MARK: binary_search()
 */
ssize_t binary_search(control ctrl[], size_t lft, size_t rgt, enum ascii_ctrl seek) {

  ssize_t found = -1;  //  -1 indicates value not found

  while (lft <= rgt) {
    ssize_t mm = lft + (rgt - lft) / 2;

    //  TODO: for debugging
    control ktrl = ctrl[mm];
    enum ascii_ctrl kv = ktrl.ctrl;

    //  check if seek is present at midpoint
    if (kv == seek) {
      found = mm;
      break;
    }

    if (kv < seek) {
      //  if seek is greater, ignore left half
      lft = mm + 1;
    }
    else {
      //  if seek is smaller, ignore left half
      rgt = mm - 1;
    }
  }

  return found;
}
