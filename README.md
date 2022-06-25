# Cripto-TPE

## Docker

#### make docker : 
Construye y corre el container

#### make start : 
Comienza el container

#### make stop : 
Para el container

## Ejecución
make all

### Distintos modos de ejecución

#### make EMLSB1 : 
Aplica el embed con lsb1 con el ejemplo guardando en ejemplo2022/lado.bmp se le "incrusta" itba.png y la respuesta se guarda en lsb1_embed.bmp 

#### make EXLSB1 : 
Aplica el extract con lsb1 con el ejemplo lsb1_embed.bmp, generando como salida el archivo lsb1_extract

#### make EMLSB4 : 
Aplica el embed con lsb4 con el ejemplo guardando en ejemplo2022/lado.bmp se le "incrusta" itba.png y la respuesta se guarda en lsb4_embed.bmp 

#### make EXLSB4 :
Aplica el extract con lsb4 con el ejemplo lsb1_embed.bmp, generando como salida el archivo lsb4_extract

#### make EMLSBI : 
Aplica el embed con lsbi con el ejemplo guardando en ejemplo2022/lado.bmp se le "incrusta" itba.png y la respuesta se guarda en lsbi_embed.bmp 

#### make EXLSBI :
Aplica el extract con lsbi con el ejemplo lsb1_embed.bmp, generando como salida el archivo lsbi_extract

### Archivos de prueba

#### make LSB1 :
Aplica el extract con lsb1 con el ejemplo ejemplo2022/ladoLSB1.bmp, generando como salida el archivo lsb1

### make LSB4 :
Aplica el extract con lsb4 con el ejemplo ejemplo2022/ladoLSB4.bmp, generando como salida el archivo lsb4

#### make LSBI :
Aplica el extract con lsbi con el ejemplo ejemplo2022/ladoLSBI.bmp, generando como salida el archivo lsbi

#### make DECLSB1192CBC :
Aplica el extract con lsb1 con el ejemplo ejemplo2022/ladoLSB1aes192cbc.bmp, desencriptando con AES192, modo CBS, password "escondite", y generando como salida el archivo lsb1

#### make DECLSBI256OFB :
Aplica el extract con lsbi con el ejemplo ejemplo2022/ladoLSBIaes256ofb.bmp, desencriptando con AES256, modo OFB, password "secreto", y generando como salida el archivo lsbi

#### make ENCLSB1 : 
Aplica el embed con lsb1 con el ejemplo guardando en ejemplo2022/lado.bmp se le "incrusta" ejemplo2022/itba.png, encriptando con AES192, modo CBC, password "escondite", y la respuesta se guarda en lsb1_enc.bmp 

#### make DECLSB1 : 
Aplica el extract con lsb1 con el archivo lsb1_enc.bmp, desencriptando con AES192, modo CBC, password "escondite", y generando como salida el archivo lsb1_dec

#### make ENCLSB4 : 
Aplica el embed con lsb4 con el ejemplo guardando en ejemplo2022/lado.bmp se le "incrusta" ejemplo2022/itba.png, encriptando con AES192, modo CBC, password "escondite", y la respuesta se guarda en lsb4_enc.bmp 

#### make DECLSB4 : 
Aplica el extract con lsb4 con el archivo lsb4_enc.bmp, desencriptando con AES192, modo CBC, password "escondite", y generando como salida el archivo lsb4_dec

#### make ENCLSBI : 
Aplica el embed con lsbi con el ejemplo guardando en ejemplo2022/lado.bmp se le "incrusta" ejemplo2022/itba.png, encriptando con AES192, modo CBC, password "escondite", y la respuesta se guarda en lsbi_enc.bmp 

#### make DECLSBI : 
Aplica el extract con lsbi con el archivo lsbi_enc.bmp, desencriptando con AES192, modo CBC, password "escondite", y generando como salida el archivo lsbi_dec

### Extracción de archivos con métodos de estegonagrafía desconocidos

#### make MEDIANOCHE : 
Aplica el extract con lsb1 con el archivo grupo21/medianoche1.bmp, generando como salida el archivo medianoche_out

#### make KINGS : 
Aplica el extract con lsbi con el archivo grupo21/kings.bmp, generando como salida el archivo kings_out

#### make KINGS1 : 
Aplica el extract con lsb4 con el archivo grupo21/kings1.bmp, desencriptando con AES256, modo OFB, password "escondido", y generando como salida el archivo kings1_out

#### make SILENCE : 
Aplica el extract con lsb4 con el archivo grupo21/silence.bmp, desencriptando con AES128, modo OFB, password "escondido", y generando como salida el archivo silence_out