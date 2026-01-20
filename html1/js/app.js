/**
 * Webserv Test Suite - Main JavaScript
 * Test HTTP methods: GET, POST, DELETE
 * Test features: Upload, Autoindex, CGI, Errors
 */

// ========================================
// Utility Functions
// ========================================

const API = {
    baseUrl: window.location.origin,
    
    // Generic fetch wrapper
    async request(url, options = {}) {
        const startTime = performance.now();
        try {
            const response = await fetch(url, options);
            const endTime = performance.now();
            const duration = (endTime - startTime).toFixed(2);
            
            let body;
            const contentType = response.headers.get('content-type');
            if (contentType && contentType.includes('application/json')) {
                body = await response.json();
            } else {
                body = await response.text();
            }
            
            return {
                ok: response.ok,
                status: response.status,
                statusText: response.statusText,
                headers: Object.fromEntries(response.headers.entries()),
                body: body,
                duration: duration
            };
        } catch (error) {
            return {
                ok: false,
                status: 0,
                statusText: 'Network Error',
                error: error.message,
                duration: (performance.now() - startTime).toFixed(2)
            };
        }
    },
    
    // GET request
    async get(url) {
        return this.request(url, { method: 'GET' });
    },
    
    // POST request (form data)
    async post(url, data) {
        return this.request(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded'
            },
            body: new URLSearchParams(data).toString()
        });
    },
    
    // POST request (JSON)
    async postJSON(url, data) {
        return this.request(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(data)
        });
    },
    
    // POST request (file upload)
    async upload(url, formData) {
        return this.request(url, {
            method: 'POST',
            body: formData
        });
    },
    
    // DELETE request
    async delete(url) {
        return this.request(url, { method: 'DELETE' });
    },
    
    // PUT request
    async put(url, data) {
        return this.request(url, {
            method: 'PUT',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(data)
        });
    }
};

// ========================================
// Response Display
// ========================================

function displayResponse(elementId, response) {
    const element = document.getElementById(elementId);
    if (!element) return;
    
    const statusClass = response.ok ? 'status' : 'status error';
    let html = `<div class="${statusClass}">Status: ${response.status} ${response.statusText} (${response.duration}ms)</div>\n`;
    
    if (response.error) {
        html += `<div class="error">Error: ${response.error}</div>\n`;
    }
    
    if (response.headers) {
        html += `\n<div class="headers">--- Headers ---</div>\n`;
        for (const [key, value] of Object.entries(response.headers)) {
            html += `<span class="headers">${key}:</span> ${value}\n`;
        }
    }
    
    if (response.body) {
        html += `\n<div class="headers">--- Body ---</div>\n`;
        if (typeof response.body === 'object') {
            html += `<pre class="body">${JSON.stringify(response.body, null, 2)}</pre>`;
        } else {
            // Escape HTML
            const escaped = response.body
                .replace(/&/g, '&amp;')
                .replace(/</g, '&lt;')
                .replace(/>/g, '&gt;');
            html += `<pre class="body">${escaped}</pre>`;
        }
    }
    
    element.innerHTML = html;
}

// ========================================
// GET Tests
// ========================================

async function testGetIndex() {
    const response = await API.get('/');
    displayResponse('get-response', response);
}

async function testGetFile(path) {
    const response = await API.get(path);
    displayResponse('get-response', response);
}

async function testGetCustom() {
    const url = document.getElementById('get-url').value;
    if (!url) {
        alert('Veuillez entrer une URL');
        return;
    }
    const response = await API.get(url);
    displayResponse('get-response', response);
}

// ========================================
// POST Tests
// ========================================

async function testPostForm() {
    const name = document.getElementById('post-name').value;
    const email = document.getElementById('post-email').value;
    const message = document.getElementById('post-message').value;
    
    const response = await API.post('/cgi-bin/test.py', {
        name: name,
        email: email,
        message: message
    });
    displayResponse('post-response', response);
}

async function testPostJSON() {
    const jsonData = document.getElementById('post-json').value;
    try {
        const data = JSON.parse(jsonData);
        const response = await API.postJSON('/cgi-bin/test.py', data);
        displayResponse('post-response', response);
    } catch (e) {
        displayResponse('post-response', {
            ok: false,
            status: 0,
            statusText: 'JSON Parse Error',
            error: e.message,
            duration: 0
        });
    }
}

async function testPostLarge() {
    // Generate large payload
    const size = parseInt(document.getElementById('payload-size').value) || 1000;
    const data = 'X'.repeat(size);
    
    const response = await API.post('/cgi-bin/test.py', { data: data });
    displayResponse('post-response', response);
}

// ========================================
// DELETE Tests
// ========================================

async function testDelete() {
    const filename = document.getElementById('delete-filename').value;
    if (!filename) {
        alert('Veuillez entrer un nom de fichier');
        return;
    }
    const response = await API.delete('/uploads/' + filename);
    displayResponse('delete-response', response);
}

async function refreshFileList() {
    const response = await API.get('/uploads/');
    displayResponse('delete-response', response);
}

// ========================================
// File Upload
// ========================================

function initUpload() {
    const uploadArea = document.getElementById('upload-area');
    const fileInput = document.getElementById('file-input');
    
    if (!uploadArea || !fileInput) return;
    
    // Click to upload
    uploadArea.addEventListener('click', () => fileInput.click());
    
    // Drag and drop
    uploadArea.addEventListener('dragover', (e) => {
        e.preventDefault();
        uploadArea.classList.add('dragover');
    });
    
    uploadArea.addEventListener('dragleave', () => {
        uploadArea.classList.remove('dragover');
    });
    
    uploadArea.addEventListener('drop', (e) => {
        e.preventDefault();
        uploadArea.classList.remove('dragover');
        const files = e.dataTransfer.files;
        if (files.length > 0) {
            handleUpload(files[0]);
        }
    });
    
    // File input change
    fileInput.addEventListener('change', () => {
        if (fileInput.files.length > 0) {
            handleUpload(fileInput.files[0]);
        }
    });
}

async function handleUpload(file) {
    const progressBar = document.getElementById('upload-progress');
    const progressFill = progressBar?.querySelector('.progress');
    
    if (progressBar) progressBar.style.display = 'block';
    if (progressFill) progressFill.style.width = '0%';
    
    const formData = new FormData();
    formData.append('file', file);
    
    // Simulate progress (real progress needs XMLHttpRequest)
    let progress = 0;
    const interval = setInterval(() => {
        progress += 10;
        if (progressFill) progressFill.style.width = progress + '%';
        if (progress >= 90) clearInterval(interval);
    }, 100);
    
    const response = await API.upload('/uploads/', formData);
    
    clearInterval(interval);
    if (progressFill) progressFill.style.width = '100%';
    
    displayResponse('upload-response', response);
    
    // Refresh file list after upload
    setTimeout(() => {
        if (progressBar) progressBar.style.display = 'none';
        listUploadedFiles();
    }, 500);
}

async function listUploadedFiles() {
    const response = await API.get('/uploads/');
    const listElement = document.getElementById('uploaded-files-list');
    
    if (!listElement) return;
    
    if (response.ok && response.body) {
        // Parse HTML response to extract file links
        const parser = new DOMParser();
        const doc = parser.parseFromString(response.body, 'text/html');
        const links = doc.querySelectorAll('a');
        
        let html = '';
        links.forEach(link => {
            const filename = link.textContent;
            if (filename && filename !== '../' && filename !== './') {
                html += `
                    <li>
                        <span class="file-name">
                            <span class="file-icon">📄</span>
                            <a href="/uploads/${filename}" target="_blank">${filename}</a>
                        </span>
                        <button class="btn btn-danger btn-sm" onclick="deleteFile('${filename}')">
                            🗑️ Supprimer
                        </button>
                    </li>
                `;
            }
        });
        
        listElement.innerHTML = html || '<li>Aucun fichier</li>';
    } else {
        listElement.innerHTML = '<li>Impossible de charger la liste</li>';
    }
}

async function deleteFile(filename) {
    if (!confirm(`Supprimer ${filename} ?`)) return;
    
    const response = await API.delete('/uploads/' + filename);
    displayResponse('upload-response', response);
    
    // Refresh file list
    listUploadedFiles();
}

// ========================================
// Error Tests
// ========================================

async function testError(code) {
    let url;
    switch (code) {
        case 400:
            // Bad request - send malformed data
            url = '/%ZZ%invalid';
            break;
        case 403:
            url = '/forbidden/';
            break;
        case 404:
            url = '/this-page-does-not-exist-' + Date.now();
            break;
        case 405:
            // Method not allowed - try PUT if not supported
            const putResponse = await API.put('/index.html', {});
            displayResponse('error-response', putResponse);
            return;
        case 413:
            // Payload too large - send huge data
            const bigData = 'X'.repeat(10000000); // 10MB
            const largeResponse = await API.post('/', { data: bigData });
            displayResponse('error-response', largeResponse);
            return;
        case 500:
            url = '/cgi-bin/error500.py';
            break;
        case 501:
            // Not implemented - try unsupported method
            const response = await API.request('/', { method: 'PATCH' });
            displayResponse('error-response', response);
            return;
        case 505:
            url = '/test-505';
            break;
        default:
            url = `/errors/${code}.html`;
    }
    
    const response = await API.get(url);
    displayResponse('error-response', response);
}

// ========================================
// CGI Tests
// ========================================

async function testCGI(type) {
    let url;
    switch (type) {
        case 'python':
            url = '/cgi-bin/test.py';
            break;
        case 'php':
            url = '/cgi-bin/test.php';
            break;
        case 'perl':
            url = '/cgi-bin/test.pl';
            break;
        case 'shell':
            url = '/cgi-bin/test.sh';
            break;
        case 'ruby':
            url = '/cgi-bin/test.rb';
            break;
        default:
            url = '/cgi-bin/test.py';
    }
    
    const response = await API.get(url + '?test=hello&name=Webserv');
    displayResponse('cgi-response', response);
}

async function testCGIPost(type) {
    let url = `/cgi-bin/test.${type}`;
    const response = await API.post(url, {
        method: 'POST',
        data: 'Test CGI POST',
        timestamp: new Date().toISOString()
    });
    displayResponse('cgi-response', response);
}

// ========================================
// Autoindex Test
// ========================================

async function testAutoindex(path) {
    const response = await API.get(path || '/uploads/');
    displayResponse('autoindex-response', response);
}

// ========================================
// Run All Tests
// ========================================

async function runAllTests() {
    const results = document.getElementById('all-tests-results');
    if (!results) return;
    
    results.innerHTML = '<div class="spinner"></div> Running tests...';
    
    const tests = [
        { name: 'GET /', fn: () => API.get('/') },
        { name: 'GET /index.html', fn: () => API.get('/index.html') },
        { name: 'POST /cgi-bin/test.py', fn: () => API.post('/cgi-bin/test.py', { test: 'data' }) },
        { name: 'GET 404', fn: () => API.get('/nonexistent-' + Date.now()) },
        { name: 'GET /uploads/ (autoindex)', fn: () => API.get('/uploads/') },
    ];
    
    let html = '<h3>Test Results</h3>';
    let passed = 0;
    let failed = 0;
    
    for (const test of tests) {
        try {
            const response = await test.fn();
            const success = response.ok || (test.name.includes('404') && response.status === 404);
            
            if (success) {
                passed++;
                html += `<div class="test-result pass">
                    <span class="icon">✅</span>
                    <span>${test.name}</span>
                    <span class="badge badge-success">${response.status} - ${response.duration}ms</span>
                </div>`;
            } else {
                failed++;
                html += `<div class="test-result fail">
                    <span class="icon">❌</span>
                    <span>${test.name}</span>
                    <span class="badge badge-danger">${response.status} ${response.statusText}</span>
                </div>`;
            }
        } catch (e) {
            failed++;
            html += `<div class="test-result fail">
                <span class="icon">❌</span>
                <span>${test.name}</span>
                <span class="badge badge-danger">Error: ${e.message}</span>
            </div>`;
        }
    }
    
    html += `<hr><p><strong>Results:</strong> ${passed} passed, ${failed} failed</p>`;
    results.innerHTML = html;
}

// ========================================
// Session Test
// ========================================

async function testSession() {
    const response = await API.get('/cgi-bin/session.py');
    displayResponse('session-response', response);
}

// ========================================
// Chunked Transfer Test
// ========================================

async function testChunked() {
    const response = await API.get('/cgi-bin/test.py?chunked=1');
    displayResponse('chunked-response', response);
}

// ========================================
// Tabs
// ========================================

function initTabs() {
    document.querySelectorAll('.tab').forEach(tab => {
        tab.addEventListener('click', () => {
            const tabGroup = tab.closest('.card');
            const tabId = tab.dataset.tab;
            
            // Update tab buttons
            tabGroup.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
            tab.classList.add('active');
            
            // Update tab content
            tabGroup.querySelectorAll('.tab-content').forEach(content => {
                content.classList.remove('active');
                if (content.id === tabId) {
                    content.classList.add('active');
                }
            });
        });
    });
}

// ========================================
// Initialize
// ========================================

document.addEventListener('DOMContentLoaded', () => {
    initUpload();
    initTabs();
    
    // Load uploaded files if on upload page
    if (document.getElementById('uploaded-files-list')) {
        listUploadedFiles();
    }
    
    console.log('Webserv Test Suite initialized');
});
