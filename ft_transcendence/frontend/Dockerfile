# 開発用。Node 24 に固定（_docs/dev/tools.html の方針どおり期間中は上げない）。
FROM node:24-alpine

WORKDIR /app

COPY package.json package-lock.json ./
RUN npm ci

COPY . .

EXPOSE 5173
CMD ["npm", "run", "dev", "--", "--host", "0.0.0.0", "--port", "5173"]
