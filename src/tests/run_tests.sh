#!/usr/bin/env bash

set -e

echo "== Compilando proyecto =="
cd "$(dirname "$0")/.."
make

echo
echo "== Prueba 1: archivo de texto =="

echo "[1.1] Comprimiendo single_text.md..."
./huffenc -c -i tests/single_text.md -o tests/out1

echo "[1.2] Descomprimiendo out1..."
./huffenc -d -i tests/out1 -o tests/out1_dec

echo "[1.3] Mostrando primeras líneas del archivo original:"
head tests/single_text.md

echo "[1.4] Mostrando primeras líneas del archivo recuperado:"
head tests/out1_dec

echo "[1.5] Comparando con diff..."
diff tests/single_text.md tests/out1_dec && echo "OK: single_text.md"

echo
echo "== Prueba 2: archivo binario =="

echo "[2.1] Comprimiendo single_binary.jpg..."
./huffenc -c -i tests/single_binary.jpg -o tests/out2

echo "[2.2] Descomprimiendo out2..."
./huffenc -d -i tests/out2 -o tests/out2_dec

echo "[2.3] Mostrando hexdump original (primeros 32 bytes):"
hexdump -C tests/single_binary.jpg | head

echo "[2.4] Mostrando hexdump recuperado (primeros 32 bytes):"
hexdump -C tests/out2_dec | head

echo "[2.5] Comparando binarios..."
diff tests/single_binary.jpg tests/out2_dec && echo "OK: single_binary.jpg"

echo
echo "== Prueba 3: directorio completo (concurrencia + compresión + encriptación) =="

echo "[3.0] Limpiando salidas anteriores..."
rm -rf tests/out_dir1 tests/out_dir1_rec
mkdir -p tests/out_dir1 tests/out_dir1_rec

echo "[3.1] Procesando dir_test con -c -e..."
./huffenc -c -e -k clave --comp-alg huffman --enc-alg vigenere \
  -i tests/dir_test -o tests/out_dir1

echo "[3.2] Mostrando tamaños de archivos comprimidos:"
ls -lh tests/out_dir1

echo "[3.3] Procesando out_dir1 con -u -d..."
./huffenc -u -d -k clave --comp-alg huffman --enc-alg vigenere \
  -i tests/out_dir1 -o tests/out_dir1_rec

echo "[3.4] Comparando directorios..."
diff -r tests/dir_test tests/out_dir1_rec && echo "OK: dir_test (pipeline completo)"

echo
echo "== Prueba 4: directorio no recursivo =="

echo "[4.0] Limpiando salidas previas..."
rm -rf tests/out_dir2
mkdir -p tests/out_dir2

echo "[4.1] Comprimiendo deep_test..."
./huffenc -c -i tests/deep_test -o tests/out_dir2

echo "[4.2] Mostrando contenido de salida:"
ls tests/out_dir2

echo "[4.3] Verificando que solo root.md fue procesado..."
if [ "$(ls tests/out_dir2)" = "root.md" ]; then
  echo "OK: solo se procesó root.md (no recursivo)"
else
  echo "ADVERTENCIA: se procesaron más archivos de los esperados"
fi

echo
echo "== Prueba 5: archivo vacío =="

echo "[5.1] Comprimiendo empty.txt..."
./huffenc -c -i tests/empty.txt -o tests/out_empty

echo "[5.2] Descomprimiendo..."
./huffenc -d -i tests/out_empty -o tests/out_empty_dec

echo "[5.3] Comparando..."
diff tests/empty.txt tests/out_empty_dec && echo "OK: empty.txt"

echo
echo "== Prueba 6: archivo muy pequeño (1 byte) =="

echo "A" > tests/one_char.txt

echo "[6.1] Comprimiendo..."
./huffenc -c -i tests/one_char.txt -o tests/out_char

echo "[6.2] Mostrando cabecera HFUF del comprimido:"
xxd tests/out_char | head

echo "[6.3] Descomprimiendo..."
./huffenc -d -i tests/out_char -o tests/out_char_dec

echo "[6.4] Mostrando archivo recuperado:"
cat tests/out_char_dec

echo "[6.5] Comparando..."
diff tests/one_char.txt tests/out_char_dec && echo "OK: one_char.txt"

echo
echo "== TODAS LAS PRUEBAS FINALIZARON EXITOSAMENTE =="