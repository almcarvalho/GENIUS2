//Programa: Teste de Display LCD 16 x 2
//Autor: FILIPEFLOP

//Carrega a biblioteca LiquidCrystal
#include <LiquidCrystal.h>
#include <EEPROM.h>

//Define os pinos que serão utilizados para ligação ao display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define NOTE_D4 294

#define NOTE_G4 392

#define NOTE_A4 440

#define NOTE_A5 880

// criando o array para os 4 sons para sortear um som

int tons[4] = { NOTE_A5, NOTE_A4, NOTE_G4, NOTE_D4 };

// Nossa sequência de até 100 itens vai começar vazia.

int sequencia[100] = {};

// Indica a rodada atual que o jogo se encontra.

int rodada_atual = 0;

// Indica o passo atual dentro da sequência, é usado enquanto a sequência

// está sendo reproduzida.

int passo_atual_na_sequencia = 0;

uint8_t recorde = 0;

int maior = 0;

/*

  Indica o pino de áudio, leds e botões.

  Os pinos de leds e botões estão em ordem, relacionados uns aos outros, ou

  seja, o primeiro led está relacionado ao primeiro botão. A ordem destas

  sequências também estão relacionadas a ordem dos tons.

*/

int pinoAudio = 10;

int pinosLeds[4] = { 6, 7, 8, 9 };

int pinosBotoes[4] = { A0, A1, A2, A3 };

// Indica se um botão foi pressionado durante o loop principal.

int botao_pressionado = 0;

// Flag indicando se o jogo acabou.

int perdeu_o_jogo = false;

void setup() {
  //Define o número de colunas e linhas do LCD
  lcd.begin(16, 2);

  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(3, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("UPDATED BY");
  delay(2000);
  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.print("MONSTRAO");
  delay(2000);
  lcd.clear();

  lcd.print("RECORDE ATUAL");
  lcd.setCursor(3, 1);
  recorde = EEPROM.read(0);
  recorde = recorde == 0xff ? 0x0 : recorde;
  lcd.print(recorde);


  // Definindo o modo dos pinos dos Leds como saída.

  for (int i = 0; i <= 3; i++) {

    pinMode(pinosLeds[i], OUTPUT);

  }

  // Definindo o modo dos pinos dos Botões como entrada.
  for (int i = 0; i <= 3; i++) {
    pinMode(pinosBotoes[i], INPUT);
  }

  // Definindo o modo do pino de Áudio como saída.

  pinMode(pinoAudio, OUTPUT);

  // Inicializando o random através de uma leitura da porta analógica.
  // Esta leitura gera um valor variável entre 0 e 1023.

  randomSeed(analogRead(4)); //MUUUUUDEI DE 0 PARA 4
  delay(2000);
}

void loop() {
  atualizarPlacar2();

  // Se perdeu o jogo reinicializamos todas as variáveis.

  if (perdeu_o_jogo) {

    int sequencia[100] = {};

    rodada_atual = 0;
    passo_atual_na_sequencia = 0;

    perdeu_o_jogo = false;

    if (maior > recorde) {
      recorde = maior;
      EEPROM.write(0, recorde);
      atualizarPlacar();
    } else {
      maior = 0;
      atualizarPlacar2();
    }
  }

  // Toca um som de início para anúnicar que o jogo está começando quando é a primeira rodada.
  if (rodada_atual == 0) {

    tocarSomDeInicio();

    delay(500);

  }

  // Chama a função que inicializa a próxima rodada.

  proximaRodada();

  // Reproduz a sequência atual.

  reproduzirSequencia();

  // Aguarda os botões serem pressionados pelo jogador.

  aguardarJogador();

  // Aguarda 1 segundo entre cada jogada.
  if(!perdeu_o_jogo)
    maior++;

  delay(1000);
}

// Sorteia um novo item e adiciona na sequência.

void proximaRodada() {

  int numero_sorteado = random(0, 4);

  sequencia[rodada_atual++] = numero_sorteado;

}

// Reproduz a sequência para ser memorizada.

void reproduzirSequencia() {
  for (int i = 0; i < rodada_atual; i++) {

    tone(pinoAudio, tons[sequencia[i]]);

    digitalWrite(pinosLeds[sequencia[i]], HIGH);
    delay(500);

    noTone(pinoAudio);

    digitalWrite(pinosLeds[sequencia[i]], LOW);
    delay(100);

  }

  noTone(pinoAudio);

}

// Aguarda o jogador iniciar sua jogada.

void aguardarJogador() {

  for (int i = 0; i < rodada_atual; i++) {
    aguardarJogada();

    verificarJogada();
    if (perdeu_o_jogo) {

      break;
    }

    passo_atual_na_sequencia++;

  }

  // Redefine a variável para 0.

  passo_atual_na_sequencia = 0;
}

void aguardarJogada() {

  boolean jogada_efetuada = false;

  while (!jogada_efetuada) {

    for (int i = 0; i <= 3; i++) {

      if (digitalRead(pinosBotoes[i]) == HIGH) {

        // Dizendo qual foi o botao pressionado.

        botao_pressionado = i;

        tone(pinoAudio, tons[i]);

        digitalWrite(pinosLeds[i], HIGH);

        delay(300);

        digitalWrite(pinosLeds[i], LOW);

        noTone(pinoAudio);

        jogada_efetuada = true;

      }

    }

    delay(10);
  }
}

void verificarJogada() {
  if (sequencia[passo_atual_na_sequencia] != botao_pressionado) {
    // GAME OVER.

    for (int i = 0; i <= 3; i++) {
      tone(pinoAudio, tons[i]);
      digitalWrite(pinosLeds[i], HIGH);

      delay(200);

      digitalWrite(pinosLeds[i], LOW);
      noTone(pinoAudio);
    }

    tone(pinoAudio, tons[3]);

    for (int i = 0; i <= 3; i++) {

      digitalWrite(pinosLeds[0], HIGH);

      digitalWrite(pinosLeds[1], HIGH);

      digitalWrite(pinosLeds[2], HIGH);

      digitalWrite(pinosLeds[3], HIGH);

      delay(100);

      digitalWrite(pinosLeds[0], LOW);

      digitalWrite(pinosLeds[1], LOW);

      digitalWrite(pinosLeds[2], LOW);

      digitalWrite(pinosLeds[3], LOW);

      delay(100);

    }

    noTone(pinoAudio);

    perdeu_o_jogo = true;

  }
}

void tocarSomDeInicio() {

  tone(pinoAudio, tons[0]);

  digitalWrite(pinosLeds[0], HIGH);

  digitalWrite(pinosLeds[1], HIGH);
  digitalWrite(pinosLeds[2], HIGH);
  digitalWrite(pinosLeds[3], HIGH);

  delay(500);

  digitalWrite(pinosLeds[0], LOW);
  digitalWrite(pinosLeds[1], LOW);
  digitalWrite(pinosLeds[2], LOW);

  digitalWrite(pinosLeds[3], LOW);
  delay(500);
  noTone(pinoAudio);
}

void atualizarPlacar() {
  lcd.clear();
  lcd.print("NOVO RECORDE:");
  lcd.setCursor(3, 1);
  lcd.print(recorde); //não sei por que diabos tava mostrando 1 a mais no placar ai fiz essa pequena gambiarra pra resolver
}

void atualizarPlacar2() {
  lcd.clear();
  lcd.print("RECORDE:");
  lcd.setCursor(9, 0);
  lcd.print(recorde);
  lcd.setCursor(0,1);
  lcd.print("PONTOS:");
  lcd.setCursor(9,1);
  lcd.print(maior);
}



