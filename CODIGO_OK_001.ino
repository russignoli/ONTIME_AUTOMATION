#include <WiFi.h> //funcionalidades de conexão wifi, como AP e WebServer

//Crendenciais do ponto de acesso teste
const char *ssid = "EEEEEEUUUUUUU";
const char *senha = "123456789";
WiFiServer server(80); //Porta padrão 80



void run_html(WiFiClient client){ 
    String html_content = \
    "<!DOCTYPE html><html>" \ 
        "<head><style media='screen' type='text/css'>" \  
            "html{display:inline-block;margin:10px auto;text-align:center;}" \  
        "</style></head>" \   
        "<body>" \ 
            "<h1 style='font-size:40px'>Acionamento Bomba</h1>" \  
            "<p><a href='/LED_ON'>" \ 
                "<button style='width:200px;font-size:80px'>ON</button>" \  
            "</a></p>" \ 
            "<p><a href='/LED_OFF'>" \ 
                "<button style='width:200px;font-size:80px'>OFF</button>" \  
            "</a></p>" \ 
        "</body>" \ 
    "</html>"; 
 
    client.println(html_content); 
} 

void setup(){
    //Configura pino conectado ao LED como saída
    pinMode(33, OUTPUT); 
    //Inicializa serial
    Serial.begin(115200);
    Serial.println();

    //Configura ESP no modo AP
    Serial.printf("Configurando ponto de acesso '%s'\n", ssid);
    WiFi.softAP(ssid, senha);
    server.begin();
    Serial.println("Configuração conluída");
}

void loop() {
    //Verifica se existe algum client na rede 
    WiFiClient client = server.available();   
    //Caso positivo, imprime "New Client" no monitor
    if (client){                              
        Serial.println("New Client");  
        //Enquanto client conectado, verifica se existem bytes a serem lidos     
        //e concatena os bytes recebidos na String cabecalho
        String cabecalho;    
        while (client.connected()){     
            if (client.available()){
                cabecalho += (char)client.read();  
                //Se receber nova linha em branco, encerrou leitura dos dados
                if (cabecalho.endsWith("\n\r\n")){
                    Serial.println(cabecalho); //imprime cabeçalhos http recebidos
                    // iniciamos a resposta http com o código OK(200), 
                    // o tipo de conteúdo a ser enviado e tipo de conexão.
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type:text/html");
                    client.println("Connection: close");
                    client.println();
                    // INSIRA AQUI SUA APLICAÇÃO

                    run_html(client); 
                    if(cabecalho.indexOf("GET /LED_ON")>= 0){ 
                        digitalWrite(33, true); //Acende o LED 
                    }else if(cabecalho.indexOf("GET /LED_OFF")>= 0){ 
                        digitalWrite(33, false); //Apaga o LED 
                    } 
                                  
                    break; //sai do while loop
                }
            }
        }
        cabecalho = ""; //ao encerrar conexão, limpa variável cabecalho
        client.flush(); client.stop();
        Serial.println("Client desconectado."); Serial.println();
    }
}
