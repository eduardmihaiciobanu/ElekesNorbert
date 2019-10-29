# Placă de dezvoltare generală

## _**Descriere**_

  Proiectul constă în realizarea unei plăci generice, care poate fi conectat la orice placă cu microcontroller. Scopul este de a ajuta programatorul embedded la testarea codului, înainte ca produsul să fie realizat fizic. Astfel, programatorul poate să descopere și să corecteze greșeli sau erori care sunt datorate soft-ului, fără să așteaptă realizarea fizică a produsului. În acest fel, dezvoltarea hardware-ului și a software-ului poate să fie realizată în paralel. De asemenea, placa poate să fie folosită de studenți sau angajați noi, pentru a învăța programarea embedded, folosind diferite microcontrollere.
  Placa generică va conține o multitudine de senzori și actuatoare des folosite (de ex. encodere, afișaje, butoane, etc.), a căror ieșiri/intrări sunt legate la pinii plăcii. De asemenea, placa va conține și componentele pasive necesare pentru funcționarea senzorilor/actuatoarelor. Prin pinii de ieșire/intrare a plăcii se conectează placa cu microcontroller la placa generică.
  Pentru demonstrarea funcționalități, va fi folosit unu sau mai multe programuri care utilizează senzorii/actuatorii aflați pe placă. Programul de demonstrație va fi implementată pe o placă de dezvoltare **Atmel SAM D21 Xplained Pro**, care conține microcontrollerul **ATSAMD21J18A**. 

## _**Specificații**_

  Tensiunea de alimentare: 3V3 sau 5V

  Placa va conține:
    *1x encoder digital
    *1x afișaj 7 segment cu digiți multiplexați
    *1x afișaj LCD
    *8x butoane
    *8x LED-uri
    *1x potențiometru
    *1x convertor UART-USB
    *1x senzor Hall