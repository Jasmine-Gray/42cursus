import react from '@vitejs/plugin-react'
import tailwindcss from '@tailwindcss/vite'
import path from 'path'
import { defineConfig } from 'vitest/config'

// https://vite.dev/config/
export default defineConfig({
  plugins: [react(), tailwindcss()],
  resolve: {
    alias: {
      '@': path.resolve(import.meta.dirname, './src'),
    },
  },
  server: {
    // Caddy 経由で同じ Wi-Fi の別端末（Host が IP や *.local）から入っても vite が Host を拒否しないようにする。
    // IP は既定で許可されるので、これは mDNS 名（xxx.local）用。
    allowedHosts: ['.local'],
    // 開発時、契約のトップレベルパスを実BE(:4000)へ転送し、同一オリジン扱いにして CORS を回避する。
    // 対象パスは _tools/caddy/Caddyfile の @backend マッチャに合わせる。
    // prism を使うときは従来どおり VITE_API_BASE=http://localhost:4010 で上書きする（絶対URLなので proxy は通らない）。
    proxy: {
      '^/(health|auth|users|invites|rooms|games|friends)(/|$)': {
        target: 'http://localhost:4000',
        changeOrigin: true,
        // 画面のリロード・直接アクセス（Accept: text/html）は SPA に返す（_tools/caddy/Caddyfile の not header と同じ方針）
        bypass(req) {
          if (req.headers.accept?.includes('text/html')) {
            return '/index.html'
          }
        },
      },
      '/uploads': { target: 'http://localhost:4000', changeOrigin: true },
      '/socket': {
        target: 'http://localhost:4000',
        changeOrigin: true,
        ws: true,
      },
    },
  },
  test: {
    environment: 'jsdom',
    setupFiles: './src/test/setup.ts',
  },
})
