// --------- AYARLAR ---------
// 1. Adımda kopyaladığınız Thingspeak Kanal ID'nizi buraya girin
const KANAL_ID = 3132649; 
// -------------------------

// HTML'deki elemanları seçiyoruz
const kirlilikDegeriElementi = document.getElementById('kirlilik-degeri');
const durumMetniElementi = document.getElementById('durum-metni');
const sonGuncellemeElementi = document.getElementById('son-guncelleme');

// Thingspeak'ten veriyi çeken fonksiyon
async function veriyiCek() {
    console.log("Veri çekiliyor...");
    try {
        // Thingspeak API'sine istek atıyoruz (Kanal ID'nizle)
        // Not: Field 1'in (Kirlilik Yüzdesi) verisini çekiyoruz
        const response = await fetch(`https://api.thingspeak.com/channels/${KANAL_ID}/fields/1/last.json`);
        
        if (!response.ok) {
            throw new Error('Network yanıtı başarısız');
        }

        const data = await response.json();
        
        // Gelen veriyi (data.field1) alıyoruz
        const kirlilikYuzdesi = parseInt(data.field1);
        
        // 1. Kirlilik yüzdesini ekrana yaz
        kirlilikDegeriElementi.textContent = `% ${kirlilikYuzdesi}`;

        // 2. Yüzdeye göre durumu belirle (ESP32'deki kodun aynısı)
        let durumMetni = "";
        if (kirlilikYuzdesi <= 10) {
            durumMetni = "Temiz";
        } else if (kirlilikYuzdesi <= 30) {
            durumMetni = "Yeterince Temiz";
        } else if (kirlilikYuzdesi <= 60) {
            durumMetni = "Yeterince Temiz Degil";
        } else {
            durumMetni = "Cok Kirli";
        }
        durumMetniElementi.textContent = durumMetni;

        // 3. Son güncelleme zamanını ekrana yaz
        const guncellemeZamani = new Date(data.created_at);
        sonGuncellemeElementi.textContent = guncellemeZamani.toLocaleTimeString('tr-TR');

    } catch (error) {
        console.error('Veri çekme hatası:', error);
        durumMetniElementi.textContent = "Veri alınamadı";
    }
}

// Sayfa ilk yüklendiğinde veriyi hemen çek
veriyiCek();

// Her 20 saniyede bir veriyi tekrar çek (Thingspeak limitine uygun)
setInterval(veriyiCek, 20000);