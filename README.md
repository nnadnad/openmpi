# KELAS 02 - KELOMPOK 10

## Petunjuk Penggunaan Program
Dalam direktori root lakukan kompilasi program dengan makefile:

* Untuk melakukan kompilasi dijkstra versi serial:
`make serial`

* Untuk melakukan kompilasi dijkstra versi paralel (mpi):
`make paralel`

* Untuk menjalankan algoritma dijkstra versi serial:
`make serialrun <jumlah_proses> <nama_file_output>`

* Untuk menjalankan algoritma dijkstra versi paralel (mpi):
`make mpirun <jumlah_proses> <nama_file_output>`

## Pembagian Tugas
* 13517035 - Hilmi Naufal Yafie : Paralel DIjksta, Laporan, Eksplorasi
* 13517128 - Yudy Valentino : Serial Dijkstra, laporan, Eksplorasi

## Laporan Pengerjaan
### Deskripsi Solusi Paralel

Solusi yang kami lakukan dalam mengerjarkan Algoritma Dijkstra secara paralel adalah
dengan mengalokasikan matriks berisi jarak antar node dari graf yang dibuat ke setiap 
proses yang disediakan secara merata, dan untuk sisa pembagian dialokasikan ke proses terakhir.
Sehingga, setiap proses nantinya akan menyimpan subgraf dari graf jarak antar node secara merata 
(kecuali untuk proses terakhir). Lalu, dilakukan pencarian jarak terpendek dengan algoritma 
dijkstra dari satu node ke setiap node yang ada. Pada pencarian, pertama ditentukan dahulu 
proses mana yang dialokasikan untuk source node yang dipilih. Lalu, inisialisasi hasil dari jarak 
terpendek dengan jarak source node ke setiap node berdasarkan proses yang dipilih. Setelah itu, 
broadcast hasil jarak terpendek ke setiap proses. Menggunakan hasil jarak terpendek yang telah di inisialisasi, 
lakukan pencarian jarak terpendek dari source node ke setiap node untuk setiap proses. Nantinya,
setiap hasil dari setiap proses akan dikumpulkan dan dicari nilai terkecilnya dengan fungsi
MPI_Allreduce. Hal ini dilakukan berulang untuk setiap node. Hasil dari setiap pencarian nantinya disimpan 
ke dalam graf, lalu graf tersebut dituliskan ke output file.


### Analisis Solusi
Berdasarkan solusi yang kami terapkan, tentunya banyak cara yang menghasilkan lebih baik dalam pengujian dan lebih mudah, 
salah satunya paralel yang digunakan dibagian node yang dipecah dan algoritma dijkstra tidak mengalami paralelisasi.
Kemudian pemakaian scatter dan getter dapat mengefisiensikan dalam pengiriman message ke proses lain.


### Hasil Uji
Berikut Merupakan hasil uji yang kami lakukan untuk node 100, 500, 1000, dan 3000 baik untuk Serial Dijkstra dan Paralel Dijkstra (dalam seconds):

* **Serial Dijkstra**

| N             | Percobaan 1   | Percobaan 2 | Percobaan 3 | Rata-rata           |
| ------------- |:-------------:| -----:| ------------- |:-------------:|
| 100      | 0,053314 | 0,048412 | 0,065642      | 0.055789 |
| 500      | 29,113564      |   29,850699 | 26,956003      | 28,640089 |
| 1000 | 13,687153      |    14,105954 | 14,327809      | 14,040305 |
| 3000      | 800,615266 | 693,996518 | 661,666910      | 718,759565 |


* **Paralel Dijkstra**

| N             | Percobaan 1   | Percobaan 2 | Percobaan 3 | Rata-rata           |
| ------------- |:-------------:| -----:| ------------- |:-------------:|
| 100      | 508,32865925   | 1007,96928275 | 786,15209475      | 767.483345583 |
| 500      | 563,04427      |   484,21250025 | 1253,5861415      | 766.94763725 |
| 1000     | 1914,996241    |    1573,43390075 | 2882,69261825      | 2123.70758667 |
| 3000      | 1047,58902675 | 1522,363494 | 1008,792743      | 1192.91508792 |

* **Keterangan: Untuk proses paralel node 3000 dijalankan secara lokal dikarenakan server ssh yang tidak memungkinkan untuk dipakai oleh seluruh mahasiswa ITB**


### Analisis Uji
Berdasarkan hasil uji, paralel dijkstra menghasilkan rata-rata waktu yang lebih lama, dibandingkan dengan yang serial.
Dimana seharusnya paralel dijkstra menghasilkan waktu yang lebih cepat dibandingkan dengan serial dikarenakan menggunakan beberapa prosesor untuk membagi pekerjaan sehingga beban yang didapat lebih kecil

Serial Dijkstra bekerja secara sekuensial, setiap task berurutan diproses dengan 1 prosesor, sehingga sehrausnya lebih lama dibanding dengan paralel dijkstra dikarenakan pada paralel, task dibagi oleh beberapa prosesor sehingga mempercepat waktu. 
Waktu untuk sebuah task menyelesaikan tasknya bervariasi pada workload yang diproses.
