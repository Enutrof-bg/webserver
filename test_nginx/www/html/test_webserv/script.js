// ==================== HELPER FUNCTIONS ====================

function showResult(elementId, message, type = 'info') {
    const result = document.getElementById(elementId);
    if (result) {
        result.textContent = message;
        result.className = 'result show ' + type;
    }
}

function showLoading(elementId) {
    const result = document.getElementById(elementId);
    if (result) {
        result.innerHTML = '<span class="loading"></span> Chargement...';
        result.className = 'result show';
    }
}

// ==================== GET TESTS ====================

async function testGET(path) {
    showLoading('get-result');
    try {
        const response = await fetch(path);
        const status = response.status;
        const contentType = response.headers.get('Content-Type');
        let body = '';
        
        if (contentType && contentType.includes('text')) {
            body = await response.text();
            if (body.length > 500) body = body.substring(0, 500) + '...\n[truncated]';
        } else {
            body = `[Binary data - ${response.headers.get('Content-Length')} bytes]`;
        }
        
        showResult('get-result', 
            `Status: ${status} ${response.statusText}\n` +
            `Content-Type: ${contentType}\n` +
            `---\n${body}`,
            status >= 200 && status < 300 ? 'success' : 'error'
        );
    } catch (err) {
        showResult('get-result', `Erreur: ${err.message}`, 'error');
    }
}

async function testGETQuery() {
    const name = document.getElementById('query-name').value || 'test';
    const value = document.getElementById('query-value').value || 'value';
    const path = `/cgi-bin/test.py?${name}=${value}`;
    
    showLoading('get-query-result');
    try {
        const response = await fetch(path);
        const body = await response.text();
        showResult('get-query-result',
            `URL: ${path}\nStatus: ${response.status}\n---\n${body}`,
            response.ok ? 'success' : 'error'
        );
    } catch (err) {
        showResult('get-query-result', `Erreur: ${err.message}`, 'error');
    }
}

// ==================== POST TESTS ====================

document.addEventListener('DOMContentLoaded', () => {
    // POST urlencoded
    const formUrlencoded = document.getElementById('form-urlencoded');
    if (formUrlencoded) {
        formUrlencoded.addEventListener('submit', async (e) => {
            e.preventDefault();
            showLoading('post-urlencoded-result');
            
            const formData = new FormData(formUrlencoded);
            const urlencoded = new URLSearchParams(formData).toString();
            
            try {
                const response = await fetch('/post_test', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded'
                    },
                    body: urlencoded
                });
                const body = await response.text();
                showResult('post-urlencoded-result',
                    `Status: ${response.status}\nBody envoyé: ${urlencoded}\n---\nRéponse:\n${body}`,
                    response.ok ? 'success' : 'error'
                );
            } catch (err) {
                showResult('post-urlencoded-result', `Erreur: ${err.message}`, 'error');
            }
        });
    }

    // Upload form
    const uploadForm = document.getElementById('upload-form');
    if (uploadForm) {
        uploadForm.addEventListener('submit', async (e) => {
            e.preventDefault();
            showLoading('upload-result');
            
            const formData = new FormData(uploadForm);
            
            try {
                const response = await fetch('/upload', {
                    method: 'POST',
                    body: formData
                });
                const body = await response.text();
                showResult('upload-result',
                    `Status: ${response.status} ${response.statusText}\n---\n${body}`,
                    response.ok ? 'success' : 'error'
                );
            } catch (err) {
                showResult('upload-result', `Erreur: ${err.message}`, 'error');
            }
        });
    }

    // Upload multiple
    const uploadMultiForm = document.getElementById('upload-multi-form');
    if (uploadMultiForm) {
        uploadMultiForm.addEventListener('submit', async (e) => {
            e.preventDefault();
            showLoading('upload-multi-result');
            
            const formData = new FormData(uploadMultiForm);
            
            try {
                const response = await fetch('/upload', {
                    method: 'POST',
                    body: formData
                });
                const body = await response.text();
                showResult('upload-multi-result',
                    `Status: ${response.status}\n---\n${body}`,
                    response.ok ? 'success' : 'error'
                );
            } catch (err) {
                showResult('upload-multi-result', `Erreur: ${err.message}`, 'error');
            }
        });
    }

    // CGI POST form
    const cgiForm = document.getElementById('cgi-form');
    if (cgiForm) {
        cgiForm.addEventListener('submit', async (e) => {
            e.preventDefault();
            showLoading('cgi-post-result');
            
            const formData = new FormData(cgiForm);
            const urlencoded = new URLSearchParams(formData).toString();
            
            try {
                const response = await fetch('/cgi-bin/test.py', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded'
                    },
                    body: urlencoded
                });
                const body = await response.text();
                showResult('cgi-post-result',
                    `Status: ${response.status}\n---\n${body}`,
                    response.ok ? 'success' : 'error'
                );
            } catch (err) {
                showResult('cgi-post-result', `Erreur: ${err.message}`, 'error');
            }
        });
    }
});

async function testPOSTJSON() {
    showLoading('post-json-result');
    let jsonData = document.getElementById('json-data').value;
    
    if (!jsonData) {
        jsonData = '{"test": "value", "number": 42}';
    }
    
    try {
        JSON.parse(jsonData); // Validate JSON
    } catch (e) {
        showResult('post-json-result', `JSON invalide: ${e.message}`, 'error');
        return;
    }
    
    try {
        const response = await fetch('/post_test', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: jsonData
        });
        const body = await response.text();
        showResult('post-json-result',
            `Status: ${response.status}\nBody envoyé: ${jsonData}\n---\n${body}`,
            response.ok ? 'success' : 'error'
        );
    } catch (err) {
        showResult('post-json-result', `Erreur: ${err.message}`, 'error');
    }
}

async function testPOSTLarge() {
    showLoading('post-large-result');
    const size = parseInt(document.getElementById('data-size').value) || 1000;
    const largeData = 'X'.repeat(size);
    
    try {
        const response = await fetch('/post_test', {
            method: 'POST',
            headers: {
                'Content-Type': 'text/plain'
            },
            body: largeData
        });
        const body = await response.text();
        showResult('post-large-result',
            `Taille envoyée: ${size} octets\nStatus: ${response.status} ${response.statusText}\n---\n${body.substring(0, 500)}`,
            response.ok ? 'success' : (response.status === 413 ? 'warning' : 'error')
        );
    } catch (err) {
        showResult('post-large-result', `Erreur: ${err.message}`, 'error');
    }
}

// ==================== DELETE TESTS ====================

async function testDELETE() {
    const path = document.getElementById('delete-path').value || '/uploads/test.txt';
    showLoading('delete-result');
    
    try {
        const response = await fetch(path, {
            method: 'DELETE'
        });
        const body = await response.text();
        showResult('delete-result',
            `DELETE ${path}\nStatus: ${response.status} ${response.statusText}\n---\n${body}`,
            response.ok ? 'success' : 'error'
        );
    } catch (err) {
        showResult('delete-result', `Erreur: ${err.message}`, 'error');
    }
}

async function testDELETEForbidden() {
    showLoading('delete-forbidden-result');
    
    try {
        const response = await fetch('/test_webserv/index.html', {
            method: 'DELETE'
        });
        const body = await response.text();
        showResult('delete-forbidden-result',
            `Status: ${response.status} ${response.statusText}\n` +
            `Attendu: 403 ou 405\n---\n${body}`,
            response.status === 403 || response.status === 405 ? 'success' : 'warning'
        );
    } catch (err) {
        showResult('delete-forbidden-result', `Erreur: ${err.message}`, 'error');
    }
}

// ==================== UPLOAD TESTS ====================

async function testBigUpload() {
    showLoading('upload-big-result');
    const fileInput = document.getElementById('upload-big');
    
    if (!fileInput.files.length) {
        showResult('upload-big-result', 'Sélectionnez un fichier', 'warning');
        return;
    }
    
    const formData = new FormData();
    formData.append('bigfile', fileInput.files[0]);
    
    try {
        const response = await fetch('/upload', {
            method: 'POST',
            body: formData
        });
        const body = await response.text();
        showResult('upload-big-result',
            `Fichier: ${fileInput.files[0].name}\n` +
            `Taille: ${fileInput.files[0].size} octets\n` +
            `Status: ${response.status} ${response.statusText}\n---\n${body}`,
            response.ok ? 'success' : (response.status === 413 ? 'warning' : 'error')
        );
    } catch (err) {
        showResult('upload-big-result', `Erreur: ${err.message}`, 'error');
    }
}

// ==================== REDIRECT TESTS ====================

async function testRedirect(path) {
    const resultId = path.includes('301') || path.includes('old') ? 'redirect-301-result' : 
                     path.includes('302') || path.includes('temp') ? 'redirect-302-result' : 
                     'redirect-slash-result';
    showLoading(resultId);
    
    try {
        const response = await fetch(path, {
            redirect: 'manual'  // Don't follow redirects
        });
        
        const location = response.headers.get('Location');
        showResult(resultId,
            `URL: ${path}\n` +
            `Status: ${response.status} ${response.statusText}\n` +
            `Location: ${location || 'N/A'}\n` +
            `Type: ${response.type}`,
            response.status >= 300 && response.status < 400 ? 'success' : 'warning'
        );
    } catch (err) {
        showResult(resultId, `Erreur: ${err.message}`, 'error');
    }
}

// ==================== ERROR TESTS ====================

async function testError(code) {
    showLoading('error-result');
    
    const paths = {
        400: '/bad\x00request',
        403: '/test_webserv/no_autoindex/',
        404: '/this-page-does-not-exist-12345',
        405: '/test_webserv/',  // Will use unsupported method
        413: '/post_test',
        500: '/cgi-bin/error.py'
    };
    
    try {
        let options = {};
        if (code === 405) {
            options = { method: 'PATCH' };  // Unsupported method
        } else if (code === 413) {
            options = {
                method: 'POST',
                body: 'X'.repeat(100000000)  // 100MB
            };
        }
        
        const response = await fetch(paths[code] || '/error', options);
        const body = await response.text();
        
        showResult('error-result',
            `Test erreur ${code}\n` +
            `Status reçu: ${response.status} ${response.statusText}\n` +
            `Attendu: ${code}\n` +
            `Match: ${response.status === code ? '✅' : '❌'}\n---\n${body.substring(0, 500)}`,
            response.status === code ? 'success' : 'warning'
        );
    } catch (err) {
        showResult('error-result', `Erreur: ${err.message}`, 'error');
    }
}

async function testMalformed() {
    showLoading('malformed-result');
    
    // On ne peut pas vraiment envoyer une requête malformée via fetch
    // Ce test est plus pour documentation
    showResult('malformed-result',
        `Pour tester une requête malformée, utilisez netcat:\n\n` +
        `echo -e "GET / HTTP/1.1\\r\\n\\r\\n" | nc localhost 8080\n\n` +
        `ou\n\n` +
        `echo -e "INVALID REQUEST" | nc localhost 8080`,
        'warning'
    );
}

// ==================== CGI TESTS ====================

async function testCGIGet() {
    showLoading('cgi-get-result');
    const name = document.getElementById('cgi-name').value || 'World';
    
    try {
        const response = await fetch(`/cgi-bin/hello.py?name=${encodeURIComponent(name)}`);
        const body = await response.text();
        showResult('cgi-get-result',
            `URL: /cgi-bin/hello.py?name=${name}\n` +
            `Status: ${response.status}\n---\n${body}`,
            response.ok ? 'success' : 'error'
        );
    } catch (err) {
        showResult('cgi-get-result', `Erreur: ${err.message}`, 'error');
    }
}

async function testCGIPathInfo() {
    showLoading('cgi-pathinfo-result');
    
    try {
        const response = await fetch('/cgi-bin/test.py/extra/path/info?query=test');
        const body = await response.text();
        showResult('cgi-pathinfo-result',
            `URL: /cgi-bin/test.py/extra/path/info?query=test\n` +
            `Status: ${response.status}\n---\n${body}`,
            response.ok ? 'success' : 'error'
        );
    } catch (err) {
        showResult('cgi-pathinfo-result', `Erreur: ${err.message}`, 'error');
    }
}

async function testCGITimeout() {
    showLoading('cgi-timeout-result');
    
    try {
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 10000);
        
        const response = await fetch('/cgi-bin/slow.py', {
            signal: controller.signal
        });
        clearTimeout(timeoutId);
        
        const body = await response.text();
        showResult('cgi-timeout-result',
            `Status: ${response.status}\n---\n${body}`,
            response.ok ? 'warning' : 'success'  // On s'attend à un timeout
        );
    } catch (err) {
        if (err.name === 'AbortError') {
            showResult('cgi-timeout-result', 
                `Timeout côté client (10s)\nLe serveur devrait aussi avoir un timeout CGI`,
                'success'
            );
        } else {
            showResult('cgi-timeout-result', `Erreur: ${err.message}`, 'error');
        }
    }
}

// ==================== ADVANCED TESTS ====================

async function testKeepAlive() {
    showLoading('keepalive-result');
    const results = [];
    
    try {
        for (let i = 0; i < 5; i++) {
            const start = performance.now();
            const response = await fetch('/test_webserv/static.html');
            const end = performance.now();
            results.push({
                request: i + 1,
                status: response.status,
                time: (end - start).toFixed(2)
            });
        }
        
        showResult('keepalive-result',
            `5 requêtes successives sur la même connexion:\n\n` +
            results.map(r => `#${r.request}: ${r.status} - ${r.time}ms`).join('\n'),
            'success'
        );
    } catch (err) {
        showResult('keepalive-result', `Erreur: ${err.message}`, 'error');
    }
}

async function testChunked() {
    showLoading('chunked-result');
    
    // Simuler un envoi chunked (le navigateur gère automatiquement)
    const chunks = ['Hello', ' ', 'World', '!'];
    const body = chunks.join('');
    
    try {
        const response = await fetch('/post_test', {
            method: 'POST',
            headers: {
                'Content-Type': 'text/plain'
            },
            body: body
        });
        const responseBody = await response.text();
        showResult('chunked-result',
            `Données envoyées: "${body}"\n` +
            `Status: ${response.status}\n---\n${responseBody}`,
            response.ok ? 'success' : 'error'
        );
    } catch (err) {
        showResult('chunked-result', `Erreur: ${err.message}`, 'error');
    }
}

async function stressTest() {
    const count = parseInt(document.getElementById('stress-count').value) || 10;
    showLoading('stress-result');
    
    const results = {
        success: 0,
        failed: 0,
        totalTime: 0
    };
    
    const start = performance.now();
    
    const promises = [];
    for (let i = 0; i < count; i++) {
        promises.push(
            fetch('/test_webserv/static.html')
                .then(r => {
                    if (r.ok) results.success++;
                    else results.failed++;
                })
                .catch(() => results.failed++)
        );
    }
    
    await Promise.all(promises);
    results.totalTime = (performance.now() - start).toFixed(2);
    
    showResult('stress-result',
        `Stress Test - ${count} requêtes simultanées\n\n` +
        `✅ Succès: ${results.success}\n` +
        `❌ Échecs: ${results.failed}\n` +
        `⏱️ Temps total: ${results.totalTime}ms\n` +
        `📊 Moyenne: ${(results.totalTime / count).toFixed(2)}ms/requête`,
        results.failed === 0 ? 'success' : 'warning'
    );
}

// ==================== SMOOTH SCROLL ====================

document.querySelectorAll('nav a').forEach(anchor => {
    anchor.addEventListener('click', function(e) {
        e.preventDefault();
        const target = document.querySelector(this.getAttribute('href'));
        if (target) {
            target.scrollIntoView({
                behavior: 'smooth',
                block: 'start'
            });
        }
    });
});

console.log('🚀 Webserv Tester loaded!');
