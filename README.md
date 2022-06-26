# Cripto-TPE

## Docker

#### make docker : 
Construye y corre el container

#### make start : 
Comienza el container

#### make stop : 
Para el container

## Compilación
make all

## Ejecución
### Embed
`.\stegobmp --embed --steg <LSB1 | LSB4 | LSBI>  -p <carrier> --in <secret> --out <output filename> `

### Extract

`.\stegobmp --extract --steg <metodo de esteganografiado>  -p <carrier> --out <output filename> `

### Encripción
Tanto en 'embed' como en 'extract' si se requiere encripción se pueden setear los siguientes parametros

`.\stegobmp -a <AES128 | AES192 | AES256 | DES> -m <ECB | CFB | OFB | CBC> --pass <password>`

De no indicarse el modo de encadenamiento se asume CBC\
De no indicarse el algoritmo de encripción se asume AES128
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
Aplica el comando ```strings grupo21/silence.bmp``` y luego podemos observar que el último string es un mensaje escondido.

## Tests
Ejecutando ```make TEST``` se corre un script de python que genera cada combinación de modo de encadenamiento, método de esteganografía y método de encripción, lo ejecuta utilizando como carrier lado.bmp y como secret itba.png, luego se ejecuta el embeding, posteriormente el extract y se compara el resultado final con el archivo original (itba.png).