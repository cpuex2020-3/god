#ifndef LABELS_H_INCLUDED
#define LABELS_H_INCLUDED

struct label_list {
  char name[256];
  int32_t address;
  struct label_list *next;
};

void add_list(struct label_list** li, char name[256], int32_t address);

int32_t search_list(struct label_list *li, char *name);

void delete_list(struct label_list *li);

struct label_list* get_text_labels(char *file_name);

#endif
