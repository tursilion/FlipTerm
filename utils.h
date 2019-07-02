char *GetString(char *path, char *key, char *def="");
UINT GetInt(char *path, char *key, int def=0);

bool WriteString(char *path, char *key, char *data);
bool WriteInt(char *path, char *key, UINT data);
